#include "stdafx.h"

#include "Game.h"
#include "Vertex.h"
#include "PBRDemoScene.h"
#include "DebugScene.h"
#include "SystemManager.h"

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
Game::Game() : DXCore() {
	//Get Singleton Instances
	systemManager = SystemManager::GetInstance();
	resourceManager = ResourceManager::GetInstance();
	renderManager = RenderManager::GetInstance();
	inputManager = InputManager::GetInstance();
	lightManager = LightManager::GetInstance();
	particleManager = ParticleManager::GetInstance();
	componentManager = ComponentManager::GetInstance();

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game() {
	//delete Scene::activeScene;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init() {
	ResourceManager::SetDevicePointer(systemManager->GetDevice());
	ResourceManager::SetContextPointer(systemManager->GetContext());
	//Call init on singletons that need it
	particleManager->Init();

	//Create and init the active scene
	//Scene::activeScene = new PBRDemoScene("PBR_Demo");
	Scene::activeScene = new PBRDemoScene("PBRDemoScene");

	//Scene::activeScene = new DebugScene( "Debug Scene" );
	Scene::activeScene->Init();

	//Call start on the singletons that need it
	renderManager->Start();
	lightManager->Start();
	//Call start on the active scene
	Scene::activeScene->Start();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime) {
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	//Handle scene switching.  Has to be done at the top-level
	if (inputManager->ActionPressed("pbr_demo_scene")) {
		delete Scene::activeScene;
		Scene::activeScene = new PBRDemoScene("PBR_Demo_Scene");
		Scene::activeScene->Init();
		Scene::activeScene->Start();
	}

	inputManager->Update();
	particleManager->Update(deltaTime);
	if (Scene::activeScene != nullptr) {
		Scene::activeScene->Update(deltaTime);
	}

	//Draw everything via the system manager (the event will trickle down)
	systemManager->Draw(deltaTime);
}