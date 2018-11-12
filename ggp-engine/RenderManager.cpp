#include "stdafx.h"

#include "RenderManager.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "LightManager.h"
#include "Texture.h"
#include "Material.h"
#include "Camera.h"

using namespace DirectX;

RenderManager* RenderManager::instance = nullptr;

RenderManager* RenderManager::GetInstance() {
	if (instance == nullptr) {
		instance = new RenderManager();
	}
	return instance;
}

void RenderManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void RenderManager::Start() {
	//Load default shaders
	defaultVertexShader = resourceManager->GetVertexShader(L"VertexShader.cso");
	defaultPixelShader = resourceManager->GetPixelShader(L"PhongPShader.cso");
	//Build the texture sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//Create the sampler state
	HRESULT samplerSuccessfulLoad = ResourceManager::GetDevicePointer()->CreateSamplerState(&samplerDesc, &samplerState);
	if (samplerSuccessfulLoad != S_OK) {
		std::cout << "Sampler load error!" << std::endl;
	}
}

UINT RenderManager::AddMeshRenderer( GameObject* _gameObject) {
	MeshRenderer* tempMR = new MeshRenderer(mrUID, _gameObject);
	meshRendererUIDMap[mrUID] = tempMR;
	mrUID++;
	return mrUID - 1;
}

MeshRenderer* RenderManager::GetMeshRenderer(UINT _uniqueID) {
	auto thisMR = meshRendererUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisMR != meshRendererUIDMap.end()) {
		return thisMR->second;
	}
	return nullptr;
}

void RenderManager::DeleteMeshRenderer(UINT _uniqueID) {
	MeshRenderer* mrTemp = GetMeshRenderer(_uniqueID);
	if (mrTemp) {
		delete mrTemp;
		meshRendererUIDMap.erase(_uniqueID);
	}
}

void RenderManager::Render(ID3D11DeviceContext* _dxContext) {
	//Loop through and render every object
	std::map<UINT, MeshRenderer*>::iterator mrIterator;
	for (mrIterator = meshRendererUIDMap.begin(); mrIterator != meshRendererUIDMap.end(); ++mrIterator) {
		MeshRenderer* mrTemp = mrIterator->second;
		//If the mesh renderer doesn't have a mesh to render, continue
		if (mrTemp->GetMesh() == nullptr) { continue; }

		//Get the material's shaders
		SimpleVertexShader* vsTemp = mrTemp->GetVertexShader();
		SimplePixelShader* psTemp = mrTemp->GetPixelShader();
		Material* matTemp = mrTemp->GetMaterial();

		//Null check vertex shader
		if (vsTemp == nullptr) { vsTemp = defaultVertexShader; }

		//Upload all data to vertex shader
		vsTemp->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
		vsTemp->SetMatrix4x4("view", activeCamera->GetViewMatrix());
		vsTemp->SetMatrix4x4("projection", activeCamera->GetProjectionMatrix());
		vsTemp->SetMatrix4x4("worldInvTrans", mrTemp->GetWorldInvTransMatrix());
		vsTemp->CopyAllBufferData();

		//Null check pixel shader
		if (psTemp == nullptr) { psTemp = defaultPixelShader; }

		//Upload lighting data from the light manager
		lightManager->UploadAllLights(psTemp);

		//Upload data from the material
		matTemp->UploadPSData(activeCamera->transform.position, samplerState, psTemp);
		//Upload gamma correction
		psTemp->SetFloat("gammaModifier", gammaCorrection);
		//Copy all buffer data
		psTemp->CopyAllBufferData();
		
		//Set the shaders and draw
		vsTemp->SetShader();
		psTemp->SetShader();
		mrTemp->Draw(_dxContext);
	}
}

float RenderManager::GetGammaCorrection() {
	return gammaCorrection;
}

void RenderManager::SetGammaCorrection(float _newGamma) {
	gammaCorrection = _newGamma;
}

Camera* RenderManager::GetActiveCamera() {
	return activeCamera;
}

void RenderManager::SetActiveCamera(Camera* _newCamera) {
	activeCamera = _newCamera;
}

RenderManager::RenderManager() {
	mrUID = 0;
	//Gamma correction value should default to 2.2
	gammaCorrection = 2.2f;
	//Get an instance of the resource manager
	resourceManager = ResourceManager::GetInstance();
	lightManager = LightManager::GetInstance();
}

RenderManager::~RenderManager() {
	//Destructor is the same as clearing the singleton
	Release();
}

void RenderManager::Release() {
	//Loop through and delete every mesh renderer
	std::map<UINT, MeshRenderer*>::iterator mrIterator;
	for (mrIterator = meshRendererUIDMap.begin(); mrIterator != meshRendererUIDMap.end(); ++mrIterator) {
		MeshRenderer* mrTemp = mrIterator->second;
		if (mrTemp != nullptr) {
			delete mrTemp;
		}
	}
	//Reset mesh renderer unique ID values
	mrUID = 0;
	//Clear the map so the singleton can be reused.
	meshRendererUIDMap.clear();

	//Release the sampler state
	samplerState->Release();
}
