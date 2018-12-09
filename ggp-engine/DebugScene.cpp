#include "stdafx.h"

#include "DebugScene.h"
#include <DirectXMath.h>
#include <string>
#include "RenderManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "FlyingCamera.h"
#include "Transform.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "RigidBody.h"
#include "DirLight.h"

using namespace DirectX;
using namespace std;

void DebugScene::Init() {
	Scene::Init();

	//We only need a single material for all three for now
	Material* stoneMaterial = resourceManager->AddMaterial("stoneMat",
														   L"VertexShader.cso",
														   L"PhongPShader.cso",
														   L"assets/textures/stone01_c.png",
														   L"assets/textures/stone01_n.png",
														   L"assets/textures/stone01_s.png");
	Material* metalMaterial = resourceManager->AddMaterial("metalMat",
														   L"VertexShader.cso",
														   L"PhongPShader.cso",
														   L"assets/textures/floor01_c.png",
														   L"assets/textures/floor01_n.png",
														   L"assets/textures/floor01_s.png");

	Material* redShiny = resourceManager->AddMaterial("redShiny", L"VertexShader.cso", L"PhongPShader.cso", XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f);

	Material* blueMatte = resourceManager->AddMaterial("blueMatte", XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), 0.0f);

	Mesh* mesh1 = resourceManager->GetMesh("assets/meshes/cube.obj");
	Mesh* mesh2 = resourceManager->GetMesh("assets/meshes/helix.obj");
	Mesh* mesh3 = resourceManager->GetMesh("assets/meshes/sphere.obj");

	GameObject* PhysObject = new GameObject("PhysObject");
	AddChild(PhysObject);
	PhysObject->AddComponent<MeshRenderer>(PhysObject, mesh3, metalMaterial);

	GameObject* PhysObject2 = new GameObject("PhysObject2");
	AddChild(PhysObject2);
	PhysObject2->AddComponent<MeshRenderer>(PhysObject2, mesh3, blueMatte);
	PhysObject2->transform.position.y = 3.f;



	//Create a light
	GameObject* lightObject = new GameObject("light1");
	AddChild(lightObject);
	lightObject->AddComponent<DirLight>(lightObject, XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f), XMFLOAT3(0.3f, 0.3f, 0.3f));
	GameObject* lightObject2 = new GameObject("light2");
	AddChild(lightObject2);
	lightObject2->AddComponent<DirLight>(lightObject2, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	lightObject2->transform.position.x += 2.0f;
	lightObject2->transform.position.z -= 1.0f;

	//Create a camera
	activeCamera = new FlyingCamera("MainCamera");
	AddChild(activeCamera);
	activeCamera->Start();
	activeCamera->transform.position.z = -5.0f;
	activeCamera->CalculateViewMatrix();
}

void DebugScene::Start() {
	Scene::Start();
	renderManager->SetActiveCamera(activeCamera);
}

void DebugScene::Update(float _deltaTime) {
	Scene::Update(_deltaTime);
	// #FixForNextBuild
	// This is rough dude


	// For every physics object in the scene
	// Update the physics
	// Check collisions

}
