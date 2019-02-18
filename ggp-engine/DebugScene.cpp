#include "stdafx.h"

#include "DebugScene.h"
#include <SimpleMath.h>
#include <string>
#include "DirLight.h"
#include "FlyingCamera.h"
#include "RenderManager.h"

using namespace DirectX::SimpleMath;
using namespace std;

void DebugScene::Init() {
	Scene::Init();
	//Create a light
	GameObject* lightObject = new GameObject("light1");
	AddChild(lightObject);
	lightObject->AddComponent<DirLight>(lightObject, Color(0.05f, 0.05f, 0.05f, 1.0f), Vector3(0.3f, 0.3f, 0.3f));

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

}
