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
	ShowCursor(false);

	//Get Singleton Instances
	resourceManager = ResourceManager::GetInstance();
	renderManager = RenderManager::GetInstance();
	inputManager = InputManager::GetInstance();
	lightManager = LightManager::GetInstance();
	particleManager = ParticleManager::GetInstance();
	physicsManager = Physics::PhysicsManager::GetInstance();
	componentManager = ECS::ComponentManager::GetInstance();
    scriptManager = new Scripting::ScriptManager( dxDevice, dxContext );

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

	//Release all singletons
    if ( scriptManager != nullptr )
    {
        delete scriptManager;
        scriptManager = nullptr;
    }
	resourceManager->ReleaseInstance();
	renderManager->ReleaseInstance();
	inputManager->ReleaseInstance();
	lightManager->ReleaseInstance();
	particleManager->ReleaseInstance();
	physicsManager->ReleaseInstance();
	ECS::ComponentManager::ReleaseInstance();

	//delete post process stuff
	delete ppVS;
	delete ppExtract;
	delete ppBlur;
	bloomSampler->Release();
	ppRTV->Release();
	ppSRV->Release();
	extractRTV->Release();
	extractSRV->Release();

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
	activeScene = new PBRDemoScene("PBR_Demo");
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

	ID3D11Texture2D* ppTexture;
	dxDevice->CreateTexture2D(&textureDesc, 0, &ppTexture);
	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	dxDevice->CreateRenderTargetView(ppTexture, &rtvDesc, &ppRTV);

	// Create the Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	dxDevice->CreateShaderResourceView(ppTexture, &srvDesc, &ppSRV);
	//Done using it
	ppTexture->Release();

	ID3D11Texture2D* extractTexture;
	dxDevice->CreateTexture2D(&textureDesc, 0, &extractTexture);
	dxDevice->CreateRenderTargetView(extractTexture, &rtvDesc, &extractRTV);

	// Create the Shader Resource View
	dxDevice->CreateShaderResourceView(extractTexture, &srvDesc, &extractSRV);
	//Done using it
	extractTexture->Release();
	
	//Load Post Process shaders
	ppVS = new SimpleVertexShader(dxDevice, dxContext);
	ppVS->LoadShaderFile(L"PostProcessVS.cso");

	ppExtract = new SimplePixelShader(dxDevice, dxContext);
	ppExtract->LoadShaderFile(L"ExtractBrightPS.cso");

	ppBlur = new SimplePixelShader(dxDevice, dxContext);
	ppBlur->LoadShaderFile(L"GaussianBlur.cso");

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Call start on the singletons that need it
	renderManager->Start();
	//Call start on the active scene
	activeScene->Start();

    // Load active Lua scripts
    scriptManager->LoadScripts(activeScene);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize() {
	// Handle base-level DX resize stuff
	DXCore::OnResize();
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

	inputManager->Update();
	particleManager->Update(deltaTime);
	physicsManager->UpdatePhysics(deltaTime);
	activeScene->Update(deltaTime);
    scriptManager->Update( deltaTime );
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime) {
	// Background color (Black in this case) for clearing
	const float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	dxContext->ClearRenderTargetView(backBufferRTV, color);
	dxContext->ClearRenderTargetView(ppRTV, color);
	dxContext->ClearRenderTargetView(extractRTV, color);

	dxContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	dxContext->OMSetRenderTargets(1, &ppRTV, depthStencilView);
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


	//Particles
	dxContext->OMSetRenderTargets(1, &ppRTV, 0);
	particleManager->Render();


	//Blur-----------------
	dxContext->OMSetRenderTargets(1, &backBufferRTV, 0);
	ppVS->SetShader();
	ppBlur->SetShader();
	ppBlur->SetShaderResourceView("Pixels", extractSRV);
	ppBlur->SetShaderResourceView("Pixels2", ppSRV);
	ppBlur->SetSamplerState("Sampler", bloomSampler);
	ppBlur->SetInt("blurAmount", 15);
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


	#if defined(ENABLE_UI)
		// Create a new IMGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Draw the UI options here -----------------------------------

	// Create a window named test
	//ImGui::Begin( "Light Options" );

	//ImGui::Checkbox( "Use Dir Lights", &UseDirLights );
	//ImGui::Checkbox( "Draw Light Gizmos", &DrawLightGizmos );
	//ImGui::Checkbox( "Move Point Lights", &MovePointLights );
	//ImGui::Checkbox( "Use SkyBox", &DrawSkyBox );

	//ImGui::End();   // If you want more than one window, then use ImGui::Beigin

	ImGui::Begin("Controls");

	ImGui::Text("Mouse   - Rotate");
	ImGui::Text("WASD    - Move");
	ImGui::Text("E       - Go Up");
	ImGui::Text("Q       - Go Down");
	ImGui::Text("X       - Release Mouse");

	ImGui::End();


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
#pragma endregion