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
#include "Material.h"
#include "RigidBody.h"

using namespace DirectX;
using namespace std;

void DebugScene::Init()
{
    Scene::Init();

    //We only need a single material for all three for now
    Material* stoneMaterial = resourceManager->AddMaterial( "stoneMat",
        L"VertexShader.cso",
        L"PhongPShader.cso",
        L"assets/textures/stone01_c.png",
        L"assets/textures/stone01_n.png",
        L"assets/textures/stone01_s.png" );
    Material* metalMaterial = resourceManager->AddMaterial( "metalMat",
        L"VertexShader.cso",
        L"PhongPShader.cso",
        L"assets/textures/floor01_c.png",
        L"assets/textures/floor01_n.png",
        L"assets/textures/floor01_s.png" );

    Material* redShiny = resourceManager->AddMaterial( "redShiny", L"VertexShader.cso", L"PhongPShader.cso", XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ), 0.5f );

    Material* blueMatte = resourceManager->AddMaterial( "blueMatte", XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ), 0.0f );

    Mesh* mesh1 = resourceManager->GetMesh( "assets/meshes/cube.obj" );
    Mesh* mesh2 = resourceManager->GetMesh( "assets/meshes/helix.obj" );
    Mesh* mesh3 = resourceManager->GetMesh( "assets/meshes/sphere.obj" );

    GameObject* PhysObject = new GameObject( "PhysObject" );
    AddChild( PhysObject );

    MeshRenderer* aMesh = PhysObject->AddComponent<MeshRenderer>( PhysObject );
    if ( aMesh != nullptr )
    {
        aMesh->SetMesh( mesh3 );
        aMesh->SetMaterial( metalMaterial );
    }
    PhysObject->AddComponent<RigidBody>( PhysObject, 0.7f, EPhysicsLayer::MOVEABLE );

    GameObject* PhysObject2 = new GameObject( "PhysObject2" );
    AddChild( PhysObject2 );
    aMesh = PhysObject2->AddComponent<MeshRenderer>( PhysObject2 );
    if ( aMesh != nullptr )
    {
        aMesh->SetMesh( mesh3 );
        aMesh->SetMaterial( blueMatte );
    }
    
    PhysObject2->transform.position.y = 3.f;
    PhysObject2->AddComponent<RigidBody>( PhysObject2, 0.7f, EPhysicsLayer::MOVEABLE );


    //Create a light
    GameObject* lightObject = new GameObject( "light1" );
    AddChild( lightObject );
    lightObject->AddDirLight( XMFLOAT4( 0.05f, 0.05f, 0.05f, 1.0f ), XMFLOAT3( 0.3f, 0.3f, 0.3f ) );
    GameObject* lightObject2 = new GameObject( "light2" );
    AddChild( lightObject2 );
    lightObject2->AddPointLight( XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    //lightObject2->GetComponent<PointLight>( CompType::POINT_LIGHT )->SetAmbientStrength( 0.0f );
    //lightObject2->GetComponent<PointLight>( CompType::POINT_LIGHT )->SetExpAtten( 0.1f );
    lightObject2->transform.position.x += 2.0f;
    lightObject2->transform.position.z -= 1.0f;
    /*GameObject* lightObject3 = new GameObject( "light3" );
    AddChild( lightObject3 );
    lightObject3->AddPointLight( XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) );
    lightObject3->GetComponent<PointLight>( CompType::POINT_LIGHT )->SetAmbientStrength( 0.0f );
    lightObject3->GetComponent<PointLight>( CompType::POINT_LIGHT )->SetLinearAtten( 0.5f );
    lightObject3->transform.position.z += 1.0f;*/

    //Create a camera
    activeCamera = new FlyingCamera( "MainCamera" );
    AddChild( activeCamera );
    activeCamera->Start();
    activeCamera->transform.position.z = -5.0f;
    activeCamera->CalculateViewMatrix();
}

void DebugScene::Start()
{
    Scene::Start();
    renderManager->SetActiveCamera( activeCamera );
}

void DebugScene::Update( float _deltaTime )
{
    Scene::Update( _deltaTime );
    // #FixForNextBuild
    // This is rough dude



    // Loops throught the active rigid bodies and checks for ray hits
    /*for ( UINT i = 0; i < physicsManager->GetRigidBodyCount(); ++i )
    {
        RigidBody* _rb = physicsManager->GetRigidBody( i );
        Physics::Point rayOrigin = {};
        rayOrigin.x = activeCamera->transform.position.x;
        rayOrigin.y = activeCamera->transform.position.y;
        rayOrigin.z = activeCamera->transform.position.z;

        XMFLOAT3 rayDir = activeCamera->transform.Forward();
        float t = 0.f;
        Physics::Point hitPoint = {};

        int result = Physics::Collisions::IntersectRaySphere( rayOrigin, rayDir, _rb->GetCollider(), t, hitPoint );
        // TODO: Draw the ray
    }*/

    // For every physics object in the scene
    // Update the physics
    // Check collisions

}
