#include "Game.h"
#include "Vertex.h"
#include <iostream>
#include "MeshRenderer.h"

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

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	//delete vertexShader;
	//delete pixelShader;

	//Release all singletons
	resourceManager->ReleaseInstance();
	renderManager->ReleaseInstance();
	inputManager->ReleaseInstance();

	delete gameObject1;
	delete gameObject2;
	delete gameObject3;
	delete gameObject4;
	delete gameObject5;

	delete activeCamera;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init() {
	//Give the device and context to the resource manager
	ResourceManager::SetDevicePointer(dxDevice);
	ResourceManager::SetContextPointer(dxContext);

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Call start on the singletons that need it
	renderManager->Start();
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry() {
	//Mesh1 Vertex Array
	Vertex mesh1Verts[] = {
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	//Mesh1 indices
	int mesh1Indices[] = {0, 1, 2,
	                      2, 1, 3,
	                      2, 1, 0,
	                      3, 1, 2};

	//Mesh2 Vertex Array
	Vertex mesh2Verts[] = {
		{XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	//Mesh2 indices
	int mesh2Indices[] = {0, 2, 1};

	//Mesh3 Vertex Array
	Vertex mesh3Verts[] = {
		{XMFLOAT3(-1.0f, +2.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(+1.0f, +2.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(-1.0f, -2.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		{XMFLOAT3(+1.0f, -2.0f, +0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
	};
	//Mesh3 indices
	int mesh3Indices[] = {0, 1, 2,
						  2, 1, 3,
						  2, 1, 0,
						  3, 1, 2};

	//We only need a single material for all three for now
	Material* defaultMaterial = resourceManager->AddMaterial("defaultMat", L"VertexShader.cso", L"LambertPShader.cso");

	Mesh* mesh1 = resourceManager->GetMesh("assets/cube.obj");
	Mesh* mesh2 = resourceManager->GetMesh("assets/helix.obj");
	Mesh* mesh3 = resourceManager->GetMesh("assets/sphere.obj");

	//Create the first game object
	gameObject1 = new Spatial("Object1");
	//Give it a mesh renderer component
	gameObject1->AddMeshRenderer();
	//Give it the first mesh we made.  In the future, meshes will be managed by the MeshRenderer
	gameObject1->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh1);
	gameObject1->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(defaultMaterial);

	//Create the second game object
	gameObject2 = new Spatial("Object2");
	//We can access objects by name.  The pointer typecast is a little weird right now.  I might use overloading to auto-detect
	//the kind of object you want.  But it does work and will be immensely useful when there are a TON of game objects
	((Spatial*)GameObject::GetGameObject("Object2"))->AddMeshRenderer();
	//Give it the same mesh as Object1
	gameObject2->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh1);
	gameObject2->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(defaultMaterial);
	//Reposition and scale it differently
	gameObject2->transform.position.x -= 3.0f;
	gameObject2->transform.position.y += 1.5f;
	gameObject2->transform.scale = XMFLOAT3(0.5f, 0.5f, 0.5f);

	//Create the third game object
	//If an object is created with a duplicate unique identifier, numbers will be added to the end to make it unique
	gameObject3 = new Spatial("Object2");
	//Show that the object was given a unique ID despite being created with the same ID as object 2
	std::cout << std::endl << gameObject3->GetUniqueID() << std::endl;
	//Give object 3 a mesh
	gameObject3->AddMeshRenderer();
	gameObject3->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh2);
	gameObject3->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(defaultMaterial);
	gameObject3->transform.position.x += 3.0f;
	gameObject3->transform.rotation.z += 24.67f;

	//Create game objects 4 and 5 with the same mesh
	gameObject4 = new Spatial("Object4");
	gameObject4->AddMeshRenderer();
	gameObject4->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh3);
	gameObject4->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(defaultMaterial);
	gameObject4->transform.rotation.z += 90.0f;
	gameObject4->transform.position.y += 2.0f;
	gameObject5 = new Spatial("Object5");
	gameObject5->AddMeshRenderer();
	gameObject5->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh3);
	gameObject5->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(defaultMaterial);
	gameObject5->transform.position.y -= 2.0f;

	//Create a camera
	activeCamera = new FlyingCamera("MainCamera");
	activeCamera->Start();
	activeCamera->transform.position.z = -5.0f;
	activeCamera->CalculateViewMatrix();
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize() {
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	//TODO: Pass a resize event to the scene
	//Change the camera's aspect ratio
	activeCamera->SetAspectRatio((float)width / height);
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
	
	//TODO: Move most of this to the active scene
	activeCamera->Update(deltaTime);

	//Change object transforms every frame.  Eventually this should be done manually in a scene, or via the velocity in the physics singleton
	gameObject1->transform.rotation.z += 0.0001f;

	gameObject2->transform.rotation.z += 0.001f;
	gameObject2->transform.rotation.y += 0.0005f;
	gameObject2->transform.rotation.x += 0.001f;

	gameObject3->transform.scale.x = abs(1.0f * sin(totalTime));
	gameObject3->transform.scale.y = abs(1.0f * sin(totalTime));
	gameObject3->transform.scale.z = abs(1.0f * sin(totalTime));

	gameObject4->transform.rotation.y += 0.0002f;

	gameObject5->transform.position.x = 2.0f * sin(totalTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime) {
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

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
	renderManager->Render(dxContext, activeCamera->GetViewMatrix(), activeCamera->GetProjectionMatrix());

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

void Game::HandleMouseMove() {
	if (mouseLocked) {
		//Get the new mouse position
		GetCursorPos(&mousePos);
		//Store window rect
		RECT windowRect;
		GetWindowRect(hWnd, &windowRect);
		//Previous position is always the window's center when the cursor is locked
		prevMousePos.x = windowRect.left + width / 2;
		prevMousePos.y = windowRect.top + height / 2;
		//Get mouse displacement
		int displaceX = mousePos.x - prevMousePos.x;
		int displaceY = mousePos.y - prevMousePos.y;
		//Pass data to input manager if mouse moved
		if (displaceX != 0 || displaceY != 0) {
			inputManager->_OnMouseMove(prevMousePos.x, prevMousePos.y, mousePos.x, mousePos.y);
		}
		//Reset cursor position
		SetCursorPos(prevMousePos.x, prevMousePos.y);
	} else {
		//Store previous mouse position
		prevMousePos.x = mousePos.x;
		prevMousePos.y = mousePos.y;
		//Get new mouse position
		GetCursorPos(&mousePos);
		//Pass to input manager if there was movement
		if (prevMousePos.x != mousePos.x || prevMousePos.y != mousePos.y) {
			inputManager->_OnMouseMove(prevMousePos.x, prevMousePos.y, mousePos.x, mousePos.y);
		}
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