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
	//vertexShader = 0;
	//pixelShader = 0;

	//Get Singleton Instances
	resourceManager = ResourceManager::GetInstance();
	meshManager = MeshManager::GetInstance();
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
	meshManager->ReleaseInstance();
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
	CreateMatrices();
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Call start on the singletons that need it
	meshManager->Start();
}

// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices() {
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//    update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//    an identity matrix.  This is just to show that HLSL expects a different
	//    matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!

	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!

	//TODO: Call the Resize() event on scene
	//Give the camera the proper aspect ratio
	//activeCamera->aspectRatio = (float)width / height;
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry() {
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	//Mesh1 Vertex Array
	Vertex mesh1Verts[] = {
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), red },
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), red },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), red },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), red },
	};
	//Mesh1 indices
	int mesh1Indices[] = {0, 1, 2,
	                      2, 1, 3,
	                      2, 1, 0,
	                      3, 1, 2};

	//Mesh2 Vertex Array
	Vertex mesh2Verts[] = {
		{XMFLOAT3(-1.0f, +1.0f, +0.0f), blue},
		{XMFLOAT3(-1.0f, -1.0f, +0.0f), blue},
		{XMFLOAT3(+1.0f, -1.0f, +0.0f), blue},
	};
	//Mesh2 indices
	int mesh2Indices[] = {0, 2, 1};

	//Mesh3 Vertex Array
	Vertex mesh3Verts[] = {
		{XMFLOAT3(-1.0f, +2.0f, +0.0f), green},
		{XMFLOAT3(+1.0f, +2.0f, +0.0f), green},
		{XMFLOAT3(-1.0f, -2.0f, +0.0f), green},
		{XMFLOAT3(+1.0f, -2.0f, +0.0f), green},
	};
	//Mesh3 indices
	int mesh3Indices[] = {0, 1, 2,
						  2, 1, 3,
						  2, 1, 0,
						  3, 1, 2};

	Mesh* mesh1 = resourceManager->CreateMeshFromData(mesh1Verts, 4, mesh1Indices, 12, "mesh1");
	Mesh* mesh2 = resourceManager->CreateMeshFromData(mesh2Verts, 3, mesh2Indices, 3, "mesh2");
	Mesh* mesh3 = resourceManager->CreateMeshFromData(mesh3Verts, 4, mesh3Indices, 12, "mesh3");

	//Create the first game object
	gameObject1 = new Spatial("Object1");
	//Give it a mesh renderer component
	gameObject1->AddMeshRenderer();
	//Give it the first mesh we made.  In the future, meshes will be managed by the MeshRenderer
	gameObject1->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh1);

	//Create the second game object
	gameObject2 = new Spatial("Object2");
	//We can access objects by name.  The pointer typecast is a little weird right now.  I might use overloading to auto-detect
	//the kind of object you want.  But it does work and will be immensely useful when there are a TON of game objects
	((Spatial*)GameObject::GetGameObject("Object2"))->AddMeshRenderer();
	//Give it the same mesh as Object1
	gameObject2->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh1);
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
	gameObject3->transform.position.x += 3.0f;
	gameObject3->transform.rotation.z += 24.67f;

	//Create game objects 4 and 5 with the same mesh
	gameObject4 = new Spatial("Object4");
	gameObject4->AddMeshRenderer();
	gameObject4->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh3);
	gameObject4->transform.rotation.z += 90.0f;
	gameObject4->transform.position.y += 2.0f;
	gameObject5 = new Spatial("Object5");
	gameObject5->AddMeshRenderer();
	gameObject5->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh3);
	gameObject5->transform.position.y -= 2.0f;

	//Create a camera
	activeCamera = new Camera("MainCamera");
	activeCamera->AddInputListener();
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

	//TODO: Remove this
	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime) {
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	inputManager->Update();
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

	//Call render on the MeshManager
	meshManager->Render(dxContext, activeCamera->GetViewMatrix(), activeCamera->GetProjectionMatrix());

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y) {
	// Add any custom code here...
	inputManager->_OnMouseDown(buttonState, x, y);

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y) {
	// Add any custom code here...
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
	// Add any custom code here...
	inputManager->_OnMouseMove(x, y);

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y) {
	// Add any custom code here...
	inputManager->_OnMouseWheel(wheelDelta);
}
#pragma endregion