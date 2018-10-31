#include "DebugScene.h"
#include <DirectXMath.h>
#include <string>
#include "RenderManager.h"
#include "ResourceManager.h"
#include "Spatial.h"
#include "MeshRenderer.h"
#include "FlyingCamera.h"
#include "Transform.h"
#include "PointLight.h"
#include "Material.h"
#include "RigidBody.h"

using namespace DirectX;
using namespace std;

void DebugScene::Init() {
	Scene::Init();
    physicsManager = Physics::PhysicsManager::GetInstance();

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

    Spatial* PhysObject = new Spatial( "PhysObject" );
    AddChild( PhysObject );
    PhysObject->AddMeshRenderer();
    //Give it the first mesh we made.  In the future, meshes will be managed by the MeshRenderer
    PhysObject->GetComponent<MeshRenderer>( CompType::MESH_RENDERER )->SetMesh( mesh3 );
    PhysObject->GetComponent<MeshRenderer>( CompType::MESH_RENDERER )->SetMaterial( metalMaterial );
    PhysObject->AddRigidBody( 0.5f, EPhysicsLayer::MOVEABLE );

    Spatial* PhysObject2 = new Spatial( "PhysObject2" );
    AddChild( PhysObject2 );
    PhysObject2->AddMeshRenderer();
    //Give it the first mesh we made.  In the future, meshes will be managed by the MeshRenderer
    PhysObject2->GetComponent<MeshRenderer>( CompType::MESH_RENDERER )->SetMesh( mesh3 );
    PhysObject2->GetComponent<MeshRenderer>( CompType::MESH_RENDERER )->SetMaterial( blueMatte );
    PhysObject2->AddRigidBody( 0.5f, EPhysicsLayer::MOVEABLE );
    PhysObject2->transform.position.y = 3.f;

	//Create the first game object
	/*Spatial* gameObject1 = new Spatial("Object1");
	//Add it as a child
	AddChild(gameObject1);
	//Give it a mesh renderer component
	gameObject1->AddMeshRenderer();
	//Give it the first mesh we made.  In the future, meshes will be managed by the MeshRenderer
	gameObject1->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh1);
	gameObject1->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(metalMaterial);

	//Create the second game object
	gameObject1->AddChild(new Spatial("Object2"));
	Spatial* gameObject2 = gameObject1->GetGameObject<Spatial>("Object2");
	//We can access objects by name.  The pointer typecast is a little weird right now.  I might use overloading to auto-detect
	//the kind of object you want.  But it does work and will be immensely useful when there are a TON of game objects
	GameObject::GetGameObject<Spatial>("Object2")->AddMeshRenderer();
	//Give it the same mesh as Object1
	gameObject2->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh1);
	gameObject2->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(stoneMaterial);
	//Reposition and scale it differently
	gameObject2->transform.position.x -= 3.0f;
	gameObject2->transform.position.y += 1.5f;
	gameObject2->transform.scale = XMFLOAT3(0.5f, 0.5f, 0.5f);


	gameObject2->AddChild(new Spatial("Object2Child"));
	Spatial* gameObject2Child = (Spatial*)gameObject2->GetChild("Object2Child");
	gameObject2Child->AddMeshRenderer();
	gameObject2Child->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh3);
	gameObject2Child->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(redShiny);
	gameObject2Child->transform.position.x = 1.0f;

	//Create the third game object
	//If an object is created with a duplicate unique identifier, numbers will be added to the end to make it unique
	gameObject1->AddChild(new Spatial("Object3"));
	Spatial* gameObject3 = gameObject1->GetGameObject<Spatial>("Object3");
	//Give object 3 a mesh
	gameObject3->AddMeshRenderer();
	gameObject3->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh2);
	gameObject3->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(redShiny);
	gameObject3->transform.position.x += 3.0f;
	gameObject3->transform.rotation.z += 24.67f;

	//Create game objects 4 and 5 with the same mesh
	Spatial* gameObject4 = new Spatial("Object4");
	AddChild(gameObject4);
	gameObject4->AddMeshRenderer();
	gameObject4->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh3);
	gameObject4->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(blueMatte);
	gameObject4->transform.rotation.z += 90.0f;
	gameObject4->transform.position.y += 2.0f;
	Spatial* gameObject5 = new Spatial("Object5");
	AddChild(gameObject5);
	gameObject5->AddMeshRenderer();
	gameObject5->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMesh(mesh3);
	gameObject5->GetComponent<MeshRenderer>(CompType::MESH_RENDERER)->SetMaterial(stoneMaterial);
	gameObject5->transform.position.y -= 2.0f;
	gameObject5->transform.scale.x += 4.0f;*/

	//Create a light
	Spatial* lightObject = new Spatial("light1");
	AddChild(lightObject);
	lightObject->AddDirLight(XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f), XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), XMFLOAT3(0.0f, +0.0f, 1.0f));
	Spatial* lightObject2 = new Spatial("light2");
	AddChild(lightObject2);
	lightObject2->AddPointLight(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	lightObject2->GetComponent<PointLight>(CompType::POINT_LIGHT)->SetAmbientStrength(0.0f);
	lightObject2->GetComponent<PointLight>(CompType::POINT_LIGHT)->SetExpAtten(0.1f);
	lightObject2->transform.position.x += 2.0f;
	lightObject2->transform.position.z -= 1.0f;
	Spatial* lightObject3 = new Spatial("light3");
	AddChild(lightObject3);
	lightObject3->AddPointLight(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	lightObject3->GetComponent<PointLight>(CompType::POINT_LIGHT)->SetAmbientStrength(0.0f);
	lightObject3->GetComponent<PointLight>(CompType::POINT_LIGHT)->SetLinearAtten(0.5f);
	lightObject3->transform.position.z += 1.0f;

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
	//Change object transforms every frame.  Eventually this should be done manually in a scene, or via the velocity in the physics singleton
	/*GetGameObject<Spatial>("Object1")->transform.rotation.z += 0.1f * _deltaTime;

	GetGameObject<Spatial>("Object2")->transform.rotation.z += 1.0f * _deltaTime;
	GetGameObject<Spatial>("Object2")->transform.rotation.y += 0.5f * _deltaTime;
	GetGameObject<Spatial>("Object2")->transform.rotation.x += 1.0f * _deltaTime;

	GetGameObject<Spatial>("Object3")->transform.scale.x = abs(1.0f * sin(totalTime));
	GetGameObject<Spatial>("Object3")->transform.scale.y = abs(1.0f * sin(totalTime));
	GetGameObject<Spatial>("Object3")->transform.scale.z = abs(1.0f * sin(totalTime));

	GetGameObject<Spatial>("Object4")->transform.rotation.y += 0.2f * _deltaTime;

	GetGameObject<Spatial>("Object5")->transform.position.x = 2.0f * sin(totalTime);

	GetGameObject<Spatial>("light2")->transform.position.z = 2.0f * sin(totalTime);*/



    // For every physics object in the scene
    // Update the physics
    physicsManager->UpdatePhysics( _deltaTime );
    // Check collisions

}
