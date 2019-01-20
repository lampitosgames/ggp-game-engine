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
	// Background color (Black in this case) for clearing
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	dxContext->ClearRenderTargetView(backBufferRTV, color);

	dxContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	//Call render on the renderManager
	renderManager->Render();


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
#pragma endregion