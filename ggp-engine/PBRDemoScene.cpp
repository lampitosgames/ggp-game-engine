#include "stdafx.h"

#include "PBRDemoScene.h"
#include <SimpleMath.h>
#include "InputManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "FlyingCamera.h"
#include "Material.h"
#include "PBRMaterial.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "InputListener.h"
#include "DirLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ParticleEmitter.h"
#include "json.hpp"

using namespace DirectX::SimpleMath;
using namespace std;
using json = nlohmann::json;

void PBRDemoScene::Init() {
	Scene::Init();

	json res = resourceManager->GetRes();
	//string test = res["texture"]["ice"]["albedo"].get<string>();

	Material* blueMatte = resourceManager->AddMaterial("blueMatte", Color(0.0f, 0.0f, 1.0f, 1.0f), 0.0f);
	PBRMaterial* pbrMats[7];
	//Create a PBR material for testing
	//string albedo = res["texture"]["ice"]["albedo"].get<string>();
	//string normal = res["texture"]["ice"]["norma"].get<string>();
	//string rough = res["texture"]["ice"]["rough"].get<string>();
	//pbrMats[0] = resourceManager->GetPBRMaterial("icePBR", "VertexShader.cso", "PBRPShader.cso", albedo, normal, rough, nullptr);
	//pbrMats[1] = pbrMats[2] = pbrMats[3] = pbrMats[4] = pbrMats[5] = pbrMats[6] = pbrMats[0];
	pbrMats[1] = resourceManager->GetPBRMaterial("goldPBR", "VertexShader.cso", "PBRPShader.cso", "assets/textures/PBR/gold_a.jpg", "assets/textures/PBR/gold_n.jpg", "assets/textures/PBR/gold_r.jpg", "assets/textures/PBR/gold_m.jpg");
	pbrMats[2] = resourceManager->GetPBRMaterial("bronzePBR", "VertexShader.cso", "PBRPShader.cso", "assets/textures/PBR/bronze_a.png", "assets/textures/PBR/bronze_n.png", "assets/textures/PBR/bronze_r.png", "assets/textures/PBR/bronze_m.png");
	pbrMats[3] = resourceManager->GetPBRMaterial("woodFloorPBR", "VertexShader.cso", "PBRPShader.cso", "assets/textures/PBR/woodFloor_a.jpg", "assets/textures/PBR/woodFloor_n.jpg", "assets/textures/PBR/woodFloor_r.jpg", nullptr);
	pbrMats[4] = resourceManager->GetPBRMaterial("marblePBR", "VertexShader.cso", "PBRPShader.cso", "assets/textures/PBR/marble_a.jpg", "assets/textures/PBR/marble_n.jpg", "assets/textures/PBR/marble_r.jpg", nullptr);
	pbrMats[5] = resourceManager->GetPBRMaterial("tactilePavingPBR", "VertexShader.cso", "PBRPShader.cso", "assets/textures/PBR/tactilePaving_a.jpg", "assets/textures/PBR/tactilePaving_n.jpg", "assets/textures/PBR/tactilePaving_r.jpg", nullptr);
	pbrMats[6] = resourceManager->GetPBRMaterial("gravelPBR", "VertexShader.cso", "PBRPShader.cso", "assets/textures/PBR/gravel_a.jpg", "assets/textures/PBR/gravel_n.jpg", "assets/textures/PBR/gravel_r.jpg", nullptr);
	pbrMats[0] = pbrMats[1];
	//Load a sphere mesh
	Mesh* sphereMesh = resourceManager->GetMesh("assets/meshes/sphere.obj");
	//Mesh* sphereMesh = resourceManager->GenerateCube(1.0f, 2.0f);
	//Create a line of white spheres.
	for (UINT i = 0; i < 7; i++) {
		GameObject* newSphere = new GameObject("Sphere");
		AddChild(newSphere);
		MeshRenderer* aMeshRend = newSphere->AddComponent<MeshRenderer>(newSphere);
		if (aMeshRend) {
			aMeshRend->SetMesh(sphereMesh);
			aMeshRend->SetMaterial(pbrMats[i]);
		}

		newSphere->transform.position.x = (float)i * 1.2f;
	}

	//Create a single, white directional light
	GameObject* dirLight = new GameObject("dirLight1");
	AddChild(dirLight);
	GameObject* dirLight2 = new GameObject("dirLight2");
	AddChild(dirLight2);
	dirLight2->AddComponent<DirLight>(dirLight2, Color(1.0f, 1.0f, 1.0f, 1.0f), Vector3(-1.0f, -0.25f, 0.0f), 0.2f, 0.00f);
	GameObject* dirLight3 = new GameObject("dirLight3");
	AddChild(dirLight3);
	dirLight3->AddComponent<DirLight>(dirLight3, Color(1.0f, 1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 1.0f), 0.2f, 0.00f);

	GameObject* pointLight1 = new GameObject("pointLight1");
	AddChild(pointLight1);
	pointLight1->AddComponent<PointLight>(pointLight1, Color(1.0f, 1.0f, 1.0f, 1.0f));
	pointLight1->GetComponentType<PointLight>()->SetRange(10.0f);
	pointLight1->transform.position.x += 2.0f;
	pointLight1->transform.position.y += 4.0f;

	//Debug particle emitter
	EmitterOptions emitterOpts = EmitterOptions();
	pointLight1->AddComponent<ParticleEmitter>(pointLight1, emitterOpts);
	pointLight1->transform.rotation.x = -3.14159f/2.0f;

	GameObject* pointLight2 = new GameObject("pointLight2");
	AddChild(pointLight2);
	pointLight2->AddComponent<PointLight>(pointLight2, Color(0.6f, 0.6f, 0.6f, 1.0f));
	pointLight2->GetComponentType<PointLight>()->SetIntensity(1.0f);
	pointLight2->transform.position.x += 6.0f;
	pointLight2->transform.position.y += 3.0f;

	GameObject* spotLight1 = new GameObject("spotLight1");
	dirLight->AddChild(spotLight1);
	spotLight1->AddComponent<SpotLight>(spotLight1, Color(0.901f, 0.239f, 0.337f, 1.0f));
	spotLight1->GetComponentType<SpotLight>()->SetCone(15.0f);
	spotLight1->GetComponentType<SpotLight>()->SetRange(80.0f);
	spotLight1->GetComponentType<SpotLight>()->SetDirection(Vector3(0.0f, -1.0f, 0.0f));
	spotLight1->transform.position.y += 15.0f;

	//Create a camera
	activeCamera = new FlyingCamera("MainCamera");
	AddChild(activeCamera);
	activeCamera->Start();
	activeCamera->transform.position.z = -5.0f;
	activeCamera->CalculateViewMatrix();

	////Load terrain
	//GameObject* terrain = new GameObject("testTerrain");
	//Mesh* terrainMesh = resourceManager->GetTerrain("assets/terrain/testTerrain.raw", 513, 100.0f);
	//AddChild(terrain);
	//terrain->AddComponent<MeshRenderer>(terrain, terrainMesh, pbrMats[0]);
	//terrain->transform.position.y = -3.0f;
}

void PBRDemoScene::Start() {
	Scene::Start();
	renderManager->SetActiveCamera(activeCamera);
}

void PBRDemoScene::Update(float _deltaTime) {
	Scene::Update(_deltaTime);

	GameObject* pointLight2 = GetGameObject<GameObject>("pointLight2");
	pointLight2->transform.position.x = 3 + 3 * cos(totalTime);
	pointLight2->transform.position.z = 3 * sin(totalTime);

	if (inputManager->ActionPressed("delete_object")) {
		delete GetGameObject<GameObject>("dirLight1");
	}
}
