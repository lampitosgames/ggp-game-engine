#include "stdafx.h"

#include "Game.h"
#include "Vertex.h"
#include "PBRDemoScene.h"
#include "DebugScene.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexBuffer = 0;
	indexBuffer = 0;

	//Is the cursor locked (Default yes)
	//TODO: Make this toggle-able and have more functionality in the System singleton
	mouseLocked = true;
	ShowCursor(true);

	ppRTV = nullptr;
	extractRTV = nullptr;
	blurRTV = nullptr;
	depthRTV = nullptr;
	dofRTV = nullptr;
	dofBlurRTV = nullptr;
	ppSRV = nullptr;
	extractSRV = nullptr;
	blurSRV = nullptr;
	depthSRV = nullptr;
	dofSRV = nullptr;
	dofBlurSRV = nullptr;

	//Get Singleton Instances
	resourceManager = ResourceManager::GetInstance();
	renderManager = RenderManager::GetInstance();
	inputManager = InputManager::GetInstance();
	lightManager = LightManager::GetInstance();
	particleManager = ParticleManager::GetInstance();
	physicsManager = Physics::PhysicsManager::GetInstance();
	componentManager = ECS::ComponentManager::GetInstance();

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game() {
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (vertexBuffer) { vertexBuffer->Release(); }
	if (indexBuffer) { indexBuffer->Release(); }

	delete activeScene;
	resourceManager->ReleaseInstance();
	renderManager->ReleaseInstance();
	inputManager->ReleaseInstance();
	lightManager->ReleaseInstance();
	particleManager->ReleaseInstance();
	physicsManager->ReleaseInstance();
	ECS::ComponentManager::ReleaseInstance();

	//delete post process stuff
	//Bloom
	delete ppVS;
	delete ppExtract;
	delete ppBlur;
	delete addBlend;
	bloomSampler->Release();
	ppRTV->Release();
	ppSRV->Release();
	extractRTV->Release();
	extractSRV->Release();
	blurRTV->Release();
	blurSRV->Release();

	//DoF
	delete dofPS;
	dofSampler->Release();
	depthRTV->Release();
	depthSRV->Release();
	dofBlurRTV->Release();
	dofBlurSRV->Release();
	dofRTV->Release();
	dofSRV->Release();

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init() {
	//Give the device and context to the resource manager
	ResourceManager::SetDevicePointer(dxDevice);
	ResourceManager::SetContextPointer(dxContext);

	//Call init on singletons that need it
	particleManager->Init();

	//Create and init the active scene
	//activeScene = new PBRDemoScene("PBR_Demo");
	activeScene = new PBRDemoScene("PBRDemoScene");

	//activeScene = new DebugScene( "Debug Scene" );
	activeScene->Init();

	//Post Process - Bloom
	// Manually create a sampler state
	D3D11_SAMPLER_DESC samplerDesc1 = {}; // Zero out the struct memory
	samplerDesc1.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc1.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc1.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc1.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc1.MaxLOD = D3D11_FLOAT32_MAX;
	dxDevice->CreateSamplerState(&samplerDesc1, &bloomSampler);

	//Post Process - DoF
	// Sampler for dof
	D3D11_SAMPLER_DESC samplerDesc2 = {}; // Zero out the struct memory
	samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;
	dxDevice->CreateSamplerState(&samplerDesc2, &dofSampler);

	RegenRenderTargets();

	//Load Post Process shaders
	ppVS = new SimpleVertexShader(dxDevice, dxContext);
	ppVS->LoadShaderFile(L"PostProcessVS.cso");

	ppExtract = new SimplePixelShader(dxDevice, dxContext);
	ppExtract->LoadShaderFile(L"ExtractBrightPS.cso");

	ppBlur = new SimplePixelShader(dxDevice, dxContext);
	ppBlur->LoadShaderFile(L"GaussianBlur.cso");

	dofPS = new SimplePixelShader(dxDevice, dxContext);
	dofPS->LoadShaderFile(L"dofPS.cso");

	addBlend = new SimplePixelShader(dxDevice, dxContext);
	addBlend->LoadShaderFile(L"AdditiveBlend.cso");

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Call start on the singletons that need it
	renderManager->Start();
	//Call start on the active scene
	activeScene->Start();
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize() {
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	RegenRenderTargets();

	//Pass resize event to scene
	activeScene->OnResize((float)width, (float)height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime) {
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	//Handle mouse movement
	HandleMouseMove();

	//Handle scene switching.  Has to be done at the top-level
	if (inputManager->ActionPressed("pbr_demo_scene")) {
		delete activeScene;
		activeScene = new PBRDemoScene("PBR_Demo_Scene");
		activeScene->Init();
		activeScene->Start();
	}

	inputManager->Update();
	particleManager->Update(deltaTime);
	physicsManager->UpdatePhysics(deltaTime);
	if (activeScene != nullptr) {
		activeScene->Update(deltaTime);
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime) {
	RTarray[0] = ppRTV;
	RTarray[1] = depthRTV;
	// Background color (Black in this case) for clearing
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	dxContext->ClearRenderTargetView(backBufferRTV, color);
	dxContext->ClearRenderTargetView(ppRTV, color);
	dxContext->ClearRenderTargetView(extractRTV, color);
	dxContext->ClearRenderTargetView(blurRTV, color);
	dxContext->ClearRenderTargetView(depthRTV, color);
	dxContext->ClearRenderTargetView(dofRTV, color);
	dxContext->ClearRenderTargetView(dofBlurRTV, color);

	dxContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	dxContext->OMSetRenderTargets(2, &RTarray[0], depthStencilView);
	//Call render on the renderManager
	renderManager->Render();

	//Post Process - Bloom
	//set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dxContext->OMSetRenderTargets(1, &backBufferRTV, 0);
	dxContext->OMSetRenderTargets(1, &extractRTV, depthStencilView);
	//Extract Bright Pixels--------------------
	//set to post processing shaders
	ppVS->SetShader();
	ppExtract->SetShader();
	ppExtract->SetShaderResourceView("Pixels", ppSRV);
	ppExtract->SetSamplerState("Sampler", bloomSampler);
	ppExtract->CopyAllBufferData();

	//unbind vert/index buffer
	ID3D11Buffer* nothing = 0;
	dxContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	dxContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Draw a triangle that will hopefully fill the screen
	dxContext->Draw(3, 0);

	// Unbind this particular register
	ppExtract->SetShaderResourceView("Pixels", 0);

	//Blur Bright Pixels -----------------
	dxContext->OMSetRenderTargets(1, &backBufferRTV, 0);
	dxContext->OMSetRenderTargets(1, &blurRTV, 0);
	ppVS->SetShader();
	ppBlur->SetShader();
	ppBlur->SetShaderResourceView("Pixels", extractSRV);
	ppBlur->SetShaderResourceView("Pixels2", ppSRV);
	ppBlur->SetSamplerState("Sampler", bloomSampler);
	ppBlur->SetInt("blurAmount", blurAmount );
	ppBlur->SetFloat("pixelWidth", 1.0f / width);
	ppBlur->SetFloat("pixelHeight", 1.0f / height);
	ppBlur->CopyAllBufferData();
	//unbind vert/index buffer
	dxContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	dxContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Draw a triangle that will hopefully fill the screen
	dxContext->Draw(3, 0);

	// Unbind this particular register
	ppBlur->SetShaderResourceView("Pixels", 0);
	ppBlur->SetShaderResourceView("Pixels2", 0);

	//Particles
	//dxContext->OMSetRenderTargets(1, &ppRTV, 0);
	//particleManager->Render();

	//Additive Blend-------------------------
	dxContext->OMSetRenderTargets(1, &backBufferRTV, 0);
	dxContext->OMSetRenderTargets(1, &dofRTV, 0);
	ppVS->SetShader();
	addBlend->SetShader();
	addBlend->SetShaderResourceView("Original", ppSRV);
	addBlend->SetShaderResourceView("Blurred", blurSRV);
	addBlend->SetSamplerState("Sampler", bloomSampler);
	addBlend->CopyAllBufferData();
	//unbind vert/index buffer
	dxContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	dxContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Draw a triangle that will hopefully fill the screen
	dxContext->Draw(3, 0);

	// Unbind this particular register
	addBlend->SetShaderResourceView("Original", 0);
	addBlend->SetShaderResourceView("Blurred", 0);

	//DOF-----------------------------------------
	//Blur-----------------
	dxContext->OMSetRenderTargets(1, &backBufferRTV, 0);
	dxContext->OMSetRenderTargets(1, &dofBlurRTV, 0);
	ppVS->SetShader();
	ppBlur->SetShader();
	ppBlur->SetShaderResourceView("Pixels", dofSRV);
	ppBlur->SetShaderResourceView("Pixels2", ppSRV);
	ppBlur->SetSamplerState("Sampler", bloomSampler);
	ppBlur->SetInt("blurAmount", blurAmount );
	ppBlur->SetFloat("pixelWidth", 1.0f / width);
	ppBlur->SetFloat("pixelHeight", 1.0f / height);
	ppBlur->CopyAllBufferData();
	//unbind vert/index buffer
	dxContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	dxContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Draw a triangle that will hopefully fill the screen
	dxContext->Draw(3, 0);

	// Unbind this particular register
	ppBlur->SetShaderResourceView("Pixels", 0);
	ppBlur->SetShaderResourceView("Pixels2", 0);

	//Blur using depth buffer
	dxContext->OMSetRenderTargets(1, &backBufferRTV, 0);
	ppVS->SetShader();
	dofPS->SetShader();
	dofPS->SetFloat("pixelSize", 1.0f / (width*height));
	dofPS->SetShaderResourceView("Pixels", dofSRV);
	dofPS->SetShaderResourceView("Pixels2", dofBlurSRV);
	dofPS->SetShaderResourceView("Pixels3", depthSRV);
	dofPS->SetSamplerState("dofSampler", dofSampler);
	dofPS->CopyAllBufferData();
	//unbind vert/index buffer
	dxContext->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	dxContext->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	// Draw a triangle that will hopefully fill the screen
	dxContext->Draw(3, 0);

	// Unbind this particular register
	dofPS->SetShaderResourceView("Pixels", 0);
	dofPS->SetShaderResourceView("Pixels2", 0);
	dofPS->SetShaderResourceView("Pixels3", 0);


#if defined(ENABLE_UI)
	// Create a new IMGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Draw the UI options here -----------------------------------
    // Stats and Info ---------------------------
    {   
        ImGui::Begin( "Info" );
        ImGui::Text( "%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate );

        ImGui::Text( "%.1f FPS", ImGui::GetIO().Framerate );
        ImGui::Separator();

        ImGui::Text( "Mouse   - Rotate" );
        ImGui::Text( "WASD    - Move" );
        ImGui::Text( "E       - Go Up" );
        ImGui::Text( "Q       - Go Down" );
        ImGui::Text( "X       - Release Mouse" );

        ImGui::End();
    }

    // Demo options ---------------------------
    {
        ImGui::Begin( "Demo Options" );

        if ( ImGui::CollapsingHeader( "Blur" ) )
        {
            ImGui::DragInt( "Blur Amount", &blurAmount, 1.0f, 0, 50 );
        }

        ImGui::End();
    }

    // Draw the hierarchy of objects --------------------------
    {   
        ImGui::Begin( "Hierarchy" );

        for ( auto itr : GameObject::goUIDMap )
        {
            GameObject* obj = itr.second;

            if ( ImGui::Button( obj->GetUniqueID().c_str(), ImVec2( ImGui::GetWindowWidth(), 0.f ) ) )
            {
                SelectedObj = obj;
            }
            ImGui::Separator();
        }

        ImGui::End();
    }

    {   // Inspector --------------------------
        if ( SelectedObj != nullptr )
        {
            ImGui::Begin( "Inspector" );

            bool isActive = SelectedObj->IsActive();
            ImGui::Checkbox( "Active", &isActive ); ImGui::SameLine();
            

            ImGui::LabelText( "Name", SelectedObj->GetUniqueID().c_str() );

            ImGui::Separator();

            if ( ImGui::CollapsingHeader( "Transform" ) )
            {
                XMFLOAT3 newPos = SelectedObj->transform.position;
                ImGui::InputFloat3( "Position", ( float* ) &newPos );

                XMFLOAT3 newScale = SelectedObj->transform.scale;
                ImGui::InputFloat3( "Scale", ( float* ) &newScale );

                XMFLOAT3 newRotation = SelectedObj->transform.rotation;
                ImGui::InputFloat4( "Rotation", ( float* ) &newRotation );

                // The position of the current object
                SelectedObj->transform.position = newPos;
                SelectedObj->transform.scale =  newScale;
                SelectedObj->transform.rotation = newRotation;
            }

            ImGui::Separator();


            // Loop through each of this entity's components
            /*auto compMap = SelectedEntity->GetAllComponents();
            if ( compMap != nullptr )
            {
                for ( auto compItr = compMap->begin(); compItr != compMap->end(); ++compItr )
                {
                    ImGui::Separator();

                    ECS::IComponent* theComp = ( compItr->second );
                    if ( theComp != nullptr )
                    {
                        if ( ImGui::CollapsingHeader( theComp->ComponentName() ) )
                        {
                            theComp->DrawEditorGUI();
                        }
                    }
                }
            }*/

            ImGui::End();
        }
    }

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif


	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

void Game::HandleMouseMove() {
	mouseLocked = inputManager->GetMouseLocked();
	//Store previous mouse position
	prevMousePos.x = mousePos.x;
	prevMousePos.y = mousePos.y;
	//Get the new mouse position
	GetCursorPos(&mousePos);
	if (mouseLocked) {
		//Store window rect
		RECT windowRect;
		GetWindowRect(hWnd, &windowRect);
		//Previous position is always the window's center when the cursor is locked
		prevMousePos.x = windowRect.left + width / 2;
		prevMousePos.y = windowRect.top + height / 2;
		//Reset cursor position
		SetCursorPos(prevMousePos.x, prevMousePos.y);
	}
	//Get mouse displacement
	int displaceX = mousePos.x - prevMousePos.x;
	int displaceY = mousePos.y - prevMousePos.y;
	//Pass data to input manager if mouse moved
	if (displaceX != 0 || displaceY != 0) {
		inputManager->_OnMouseMove(prevMousePos.x, prevMousePos.y, mousePos.x, mousePos.y);
	}
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y) {
	//Pass event to the input manager
	inputManager->_OnMouseDown(buttonState, x, y);
	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}
        
// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y) {
	//Pass event to the input manager
	inputManager->_OnMouseUp(buttonState, x, y);

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y) {
	//Currently does nothing
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y) {
	//Pass event to the input manager
	inputManager->_OnMouseWheel(wheelDelta);
}
void Game::RegenRenderTargets() {
	//Release existing RTVs
	if (ppRTV) { ppRTV->Release(); ppRTV = nullptr; }
	if (extractRTV) { extractRTV->Release(); extractRTV = nullptr; }
	if (blurRTV) { blurRTV->Release(); blurRTV = nullptr; }
	if (depthRTV) { depthRTV->Release(); depthRTV = nullptr; }
	if (dofRTV) { dofRTV->Release(); dofRTV = nullptr; }
	if (dofBlurRTV) { dofBlurRTV->Release(); dofBlurRTV = nullptr; }
	//Release existing SRVs
	if (ppSRV) { ppSRV->Release(); ppSRV = nullptr; }
	if (extractSRV) { extractSRV->Release(); extractSRV = nullptr; }
	if (blurSRV) { blurSRV->Release(); blurSRV = nullptr; }
	if (depthSRV) { depthSRV->Release(); depthSRV = nullptr; }
	if (dofSRV) { dofSRV->Release(); dofSRV = nullptr; }
	if (dofBlurSRV) { dofBlurSRV->Release(); dofBlurSRV = nullptr; }

	//Create Render Target and Shader Resource Views
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	//Create a texture to base all of the resource views off of
	ID3D11Texture2D* ppTex;
	dxDevice->CreateTexture2D(&textureDesc, 0, &ppTex);
	ID3D11Texture2D* extractTex;
	dxDevice->CreateTexture2D(&textureDesc, 0, &extractTex);
	ID3D11Texture2D* blurTex;
	dxDevice->CreateTexture2D(&textureDesc, 0, &blurTex);
	ID3D11Texture2D* depthTex;
	dxDevice->CreateTexture2D(&textureDesc, 0, &depthTex);
	ID3D11Texture2D* dofTex;
	dxDevice->CreateTexture2D(&textureDesc, 0, &dofTex);
	ID3D11Texture2D* dofBlurTex;
	dxDevice->CreateTexture2D(&textureDesc, 0, &dofBlurTex);

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//Create all of the render targets
	dxDevice->CreateRenderTargetView(ppTex, &rtvDesc, &ppRTV);
	dxDevice->CreateRenderTargetView(extractTex, &rtvDesc, &extractRTV);
	dxDevice->CreateRenderTargetView(blurTex, &rtvDesc, &blurRTV);
	dxDevice->CreateRenderTargetView(depthTex, &rtvDesc, &depthRTV);
	dxDevice->CreateRenderTargetView(dofTex, &rtvDesc, &dofRTV);
	dxDevice->CreateRenderTargetView(dofBlurTex, &rtvDesc, &dofBlurRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//Create all of the shader resource views
	dxDevice->CreateShaderResourceView(ppTex, &srvDesc, &ppSRV);
	dxDevice->CreateShaderResourceView(extractTex, &srvDesc, &extractSRV);
	dxDevice->CreateShaderResourceView(blurTex, &srvDesc, &blurSRV);
	dxDevice->CreateShaderResourceView(depthTex, &srvDesc, &depthSRV);
	dxDevice->CreateShaderResourceView(dofTex, &srvDesc, &dofSRV);
	dxDevice->CreateShaderResourceView(dofBlurTex, &srvDesc, &dofBlurSRV);

	// We don't need the texture reference itself no mo'
	ppTex->Release();
	extractTex->Release();
	blurTex->Release();
	depthTex->Release();
	dofTex->Release();
	dofBlurTex->Release();
}
#pragma endregion