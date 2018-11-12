#include "stdafx.h"

#include "Scene.h"
#include "Camera.h"

Scene::Scene(std::string _uniqueID) : GameObject(_uniqueID) {
	type = GOType::SCENE;
	activeCamera = nullptr;
	totalTime = 0.0f;
}

void Scene::Init() {

}

void Scene::Start() {
	GameObject::Start();
}

void Scene::Update(float _deltaTime) {
	totalTime += _deltaTime;
	GameObject::Update(_deltaTime);
}

void Scene::OnResize(float width, float height) {
	if (activeCamera != nullptr) {
		activeCamera->SetAspectRatio(width / height);
	}
}
