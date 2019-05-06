#include "stdafx.h"

#include "PBRDemoScene.h"
#include <SimpleMath.h>
#include "json.hpp"
#include "InputManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "FlyingCamera.h"
#include "Material.h"
#include "PBRMaterial.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "InputListener.h"
#include "PointLightObj.h"
#include "SpotLightObj.h"
#include "DirLightObj.h"
#include "ParticleEmitter.h"

using namespace DirectX::SimpleMath;
using namespace std;
using json = nlohmann::json;

void PBRDemoScene::Init() {
	Scene::Init();


	Material* blueMatte = resourceManager->AddMaterial("blueMatte", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["debug"], Color(0.0f, 0.0f, 1.0f, 1.0f), 0.0f);
	Material* whiteMat = resourceManager->GetPBRMaterial("whiteMat", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["pbr"], Color(1.0f, 1.0f, 1.0f));
	PBRMaterial* pbrMats[7];

	//Create PBR mats
	pbrMats[0] = resourceManager->GetPBRMaterial(
		"icePBR",
		res["shdr"]["vert"]["default"],
		res["shdr"]["pix"]["pbr"],
		res["tex"]["ice"]["a"],
		res["tex"]["ice"]["n"],
		res["tex"]["ice"]["r"],
		res["tex"]["ice"]["m"]
	);
	pbrMats[1] = resourceManager->GetPBRMaterial("goldPBR", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["pbr"], res["tex"]["gold"]["a"], res["tex"]["gold"]["n"], res["tex"]["gold"]["r"], res["tex"]["gold"]["m"]);
	pbrMats[2] = resourceManager->GetPBRMaterial("bronzePBR", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["pbr"], res["tex"]["bronze"]["a"], res["tex"]["bronze"]["n"], res["tex"]["bronze"]["r"], res["tex"]["bronze"]["m"]);
	pbrMats[3] = resourceManager->GetPBRMaterial("woodFloorPBR", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["pbr"], res["tex"]["wood"]["a"], res["tex"]["wood"]["n"], res["tex"]["wood"]["r"], res["tex"]["wood"]["m"]);
	pbrMats[4] = resourceManager->GetPBRMaterial("marblePBR", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["pbr"], res["tex"]["marble"]["a"], res["tex"]["marble"]["n"], res["tex"]["marble"]["r"], res["tex"]["marble"]["m"]);
	pbrMats[5] = resourceManager->GetPBRMaterial("tactilePavingPBR", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["pbr"], res["tex"]["tacpav"]["a"], res["tex"]["tacpav"]["n"], res["tex"]["tacpav"]["r"], res["tex"]["tacpav"]["m"]);
	pbrMats[6] = resourceManager->GetPBRMaterial("gravelPBR", res["shdr"]["vert"]["default"], res["shdr"]["pix"]["pbr"], res["tex"]["gravel"]["a"], res["tex"]["gravel"]["n"], res["tex"]["gravel"]["r"], res["tex"]["gravel"]["m"]);
	//Load a sphere mesh
	Mesh* sphereMesh = resourceManager->GetMesh(res["mesh"]["sphere"]);
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

	//Gen a cube mesh
	Mesh* cubeMesh = resourceManager->GenerateCube(1.0f);
	GameObject* floorObj = new GameObject("Floor Cube", Vector3(0.0f, -3.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 1.0f, 100.0f));
	floorObj->AddComponent<MeshRenderer>(floorObj, cubeMesh, whiteMat);
	floorObj->GetComponentType<MeshRenderer>()->SetRecievesShadows(false);
	AddChild(floorObj);

	//Add lights (one line creation)
	AddChild(new PointLightObj("pointLight1", Vector3(2.0f, 4.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 10.0f));
	AddChild(new PointLightObj("pointLight2", Vector3(6.0f, 3.0f, 0.0f), Color(0.6f, 0.6f, 0.6f, 1.0f)));
	AddChild(new PointLightObj("pointLight3", Vector3(0.0f, -2.0f, 2.0f), Color(1.0f, 1.0f, 0.3f, 1.0f)));
	//AddChild(new SpotLightObj("spotLight1", Vector3(0.0f, 3.0f, 0.0f), Color(1.0f, 0.0f, 0.0f, 1.0f), 2.0f, Vector3(1.0f, -1.0f, 0.0f), 30.0f, 30.0f));
	AddChild(new DirLightObj("dirLight1", Color(1.0f, 1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), 0.4f, 0.05f));
	AddChild(new DirLightObj("dirLight2", Color(1.0f, 1.0f, 1.0f, 1.0f), Vector3(-1.0f, -0.25f, 0.0f), 0.4f, 0.05f));

	//Create a camera
	activeCamera = new FlyingCamera("MainCamera");
	AddChild(activeCamera);
	activeCamera->Start();
	activeCamera->transform.position.z = -5.0f;
	activeCamera->CalculateViewMatrix();

	////Load terrain
	//GameObject* terrain = new GameObject("testTerrain");
	//Mesh* terrainMesh = resourceManager->GetTerrain(res["tera"]["testTerrain"], 513, 100.0f);
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

	DirLightObj* dirLightObj = GetGameObject<DirLightObj>("dirLight1");
	dirLightObj->SetDirection(Vector3(cos(totalTime / 3.0f), sin(totalTime / 3.0f), 0.0f));

	PointLightObj* pointLight2 = GetGameObject<PointLightObj>("pointLight2");
	pointLight2->transform.position.x = 3 + 3 * cos(totalTime);
	pointLight2->transform.position.z = 3 * sin(totalTime);

	GetGameObject<PointLightObj>("pointLight1")->SetRange(5.0f + (5.0f * cos(totalTime)));
	//GetGameObject<SpotLightObj>("spotLight1")->transform.rotation.z += 1.0f * _deltaTime;
	if (inputManager->ActionPressed("delete_object")) {
		delete GetGameObject<GameObject>("dirLight1");
	}
}
