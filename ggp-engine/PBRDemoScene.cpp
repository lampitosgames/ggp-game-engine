#include "PBRDemoScene.h"
#include <DirectXMath.h>
#include "RenderManager.h"
#include "ResourceManager.h"
#include "FlyingCamera.h"
#include "Material.h"
#include "PBRMaterial.h"
#include "Mesh.h"
#include "MeshRenderer.h"

using namespace DirectX;
using namespace std;

void PBRDemoScene::Init() {
	Scene::Init();

	//Create a basic, white, shiny material
	Material* whiteShiny = resourceManager->AddMaterial("whiteShiny", L"VertexShader.cso", L"PhongPShader.cso", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
	//Create a PBR material for testing
	PBRMaterial* tactilePavingPBR = resourceManager->GetPBRMaterial("tactilePavingPBR", L"VertexShader.cso", L"PBRPShader.cso", L"assets/textures/TactilePaving_a.jpg", L"assets/textures/TactilePaving_n.jpg", L"assets/textures/TactilePaving_r.jpg", nullptr);
	//Load a sphere mesh
	Mesh* sphereMesh = resourceManager->GetMesh("assets/meshes/sphere.obj");
	//Create a line of white spheres.
	for (UINT i = 0; i < 7; i++) {
		Spatial* newSphere = new Spatial("Sphere");
		AddChild(newSphere);
		newSphere->AddMeshRenderer();
		newSphere->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(sphereMesh);
		newSphere->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(tactilePavingPBR);
		newSphere->transform.position.x = (float)i * 1.2f;
	}

	//Create a single, white directional light
	Spatial* dirLight = new Spatial("dirLight1");
	AddChild(dirLight);
	dirLight->AddDirLight(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 0.0f), 0.8f, 0.05f);

	Spatial* pointLight1 = new Spatial("pointLight1");
	AddChild(pointLight1);
	pointLight1->AddPointLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	pointLight1->GetComponent<PointLight>(CompType::POINT_LIGHT)->SetExpAtten(0.1f);
	pointLight1->transform.position.x += 2.0f;
	pointLight1->transform.position.y += 4.0f;

	Spatial* pointLight2 = new Spatial("pointLight2");
	AddChild(pointLight2);
	pointLight2->AddPointLight(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));
	pointLight2->GetComponent<PointLight>(CompType::POINT_LIGHT)->SetExpAtten(0.05f);
	pointLight2->transform.position.x += 6.0f;
	pointLight2->transform.position.y += 3.0f;

	//Create a camera
	activeCamera = new FlyingCamera("MainCamera");
	AddChild(activeCamera);
	activeCamera->Start();
	activeCamera->transform.position.z = -5.0f;
	activeCamera->CalculateViewMatrix();
}

void PBRDemoScene::Start() {
	Scene::Start();
	renderManager->SetActiveCamera(activeCamera);
}

void PBRDemoScene::Update(float _deltaTime) {
	Scene::Update(_deltaTime);

	Spatial* pointLight2 = GetGameObject<Spatial>("pointLight2");
	pointLight2->transform.position.x = 3 + 3 * cos(totalTime);
	pointLight2->transform.position.z = 3 * sin(totalTime);
}
