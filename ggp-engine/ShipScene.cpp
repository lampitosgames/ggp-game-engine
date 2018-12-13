#include "stdafx.h"


#include "ShipScene.h"
#include <DirectXMath.h>

#include "PBRDemoScene.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "FlyingCamera.h"
#include "Material.h"
#include "RigidBody.h"
#include "PBRMaterial.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "InputListener.h"
#include "DirLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "ParticleEmitter.h"

using namespace DirectX;

void ShipScene::Init()
{
    Scene::Init();

    CreateLights();

    //Create a camera
    activeCamera = new FlyingCamera( "MainCamera" );
    AddChild( activeCamera );
    activeCamera->Start();
    activeCamera->transform.position.z = -5.0f;
    activeCamera->CalculateViewMatrix();
}

void ShipScene::Start()
{
    Scene::Start();
    renderManager->SetActiveCamera( activeCamera );
}

void ShipScene::Update( float _deltaTime )
{
    Scene::Update( _deltaTime );

}

void ShipScene::CreateLights()
{
    //Create a single, white directional light
    GameObject* dirLight = new GameObject( "dirLight1" );
    AddChild( dirLight );
    GameObject* dirLight2 = new GameObject( "dirLight2" );
    AddChild( dirLight2 );
    dirLight2->AddComponent<DirLight>( dirLight2, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ), XMFLOAT3( -1.0f, -0.25f, 0.0f ), 0.2f, 0.00f );
    GameObject* dirLight3 = new GameObject( "dirLight3" );
    AddChild( dirLight3 );
    dirLight3->AddComponent<DirLight>( dirLight3, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 1.0f ), 0.2f, 0.00f );

    GameObject* pointLight1 = new GameObject( "pointLight1" );
    AddChild( pointLight1 );
    pointLight1->AddComponent<PointLight>( pointLight1, XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    pointLight1->GetComponentType<PointLight>()->SetRange( 10.0f );
    pointLight1->transform.position.x = -4.f;
    pointLight1->transform.position.y = -0.75f;

    //Debug particle emitter
	GameObject* partEmitter = new GameObject("partEmitter");
	AddChild(partEmitter);
    EmitterOptions emitterOpts = EmitterOptions();
	partEmitter->AddComponent<ParticleEmitter>(partEmitter, emitterOpts );
	partEmitter->transform.rotation.x = -3.14159f / 2.0f;
    partEmitter->transform.position.x = -4.f;
    partEmitter->transform.position.y = -0.75f;


    GameObject* pointLight2 = new GameObject( "pointLight2" );
    AddChild( pointLight2 );
    pointLight2->AddComponent<PointLight>( pointLight2, XMFLOAT4( 0.6f, 0.6f, 0.6f, 1.0f ) );
    pointLight2->GetComponentType<PointLight>()->SetIntensity( 1.0f );
    pointLight2->transform.position.x += 6.0f;
    pointLight2->transform.position.y += 3.0f;

    GameObject* spotLight1 = new GameObject( "spotLight1" );
    dirLight->AddChild( spotLight1 );
    spotLight1->AddComponent<SpotLight>( spotLight1, XMFLOAT4( 0.901f, 0.239f, 0.337f, 1.0f ) );
    spotLight1->GetComponentType<SpotLight>()->SetCone( 15.0f );
    spotLight1->GetComponentType<SpotLight>()->SetRange( 80.0f );
    spotLight1->GetComponentType<SpotLight>()->SetDirection( XMFLOAT3( 0.0f, -1.0f, 0.0f ) );
    spotLight1->transform.position.y += 15.0f;
}