#include "RenderManager.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>
#include "Spatial.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "LightManager.h"
#include "Texture.h"
#include "Material.h"

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
	defaultPixelShader = resourceManager->GetPixelShader(L"LambertPShader.cso");
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

UINT RenderManager::AddMeshRenderer(Spatial* _gameObject) {
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

void RenderManager::Render(ID3D11DeviceContext* _dxContext, DirectX::XMFLOAT4X4 _viewMatrix, DirectX::XMFLOAT4X4 _projectionMatrix, DirectX::XMFLOAT3 _camPos) {
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

		//Null check on all resources
		if (vsTemp == nullptr) { vsTemp = defaultVertexShader; }
		if (psTemp == nullptr) { psTemp = defaultPixelShader; }

		//Upload all data to vertex shader
		vsTemp->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
		vsTemp->SetMatrix4x4("view", _viewMatrix);
		vsTemp->SetMatrix4x4("projection", _projectionMatrix);
		vsTemp->SetMatrix4x4("worldInvTrans", mrTemp->GetWorldInvTransMatrix());
		//TODO: Implement a way to upload color-based material data
		//TODO: Standardize what data all shaders can accept
		vsTemp->CopyAllBufferData();

		//Upload lighting data from the light manager
		lightManager->UploadAllLights(psTemp);

		//Build texture channel toggle array
		XMINT3 channelToggle = XMINT3(matTemp->HasDiffuseTexture(),
									  matTemp->HasNormalMap(),
									  matTemp->HasSpecularMap());
		//Upload to pixel shader
		psTemp->SetData("texChanToggle", &channelToggle, sizeof(XMINT3));

		//If any textures exist, upload the sampler state
		if (channelToggle.x || channelToggle.y || channelToggle.z) {
			psTemp->SetSamplerState("basicSampler", samplerState);
		}
		//Only upload texture resources that exist
		if (channelToggle.x) { psTemp->SetShaderResourceView("diffuseTexture", matTemp->GetTexSRV()); }
		if (channelToggle.y) { psTemp->SetShaderResourceView("normalMap", matTemp->GetNormalSRV()); }
		if (channelToggle.z) { psTemp->SetShaderResourceView("specularMap", matTemp->GetSpecularSRV()); }

		//Upload material data
		psTemp->SetFloat3("cameraPosition", _camPos);
		psTemp->SetFloat4("baseColor", matTemp->GetColor());
		psTemp->SetFloat("baseSpec", matTemp->GetBaseSpecular());
		psTemp->CopyAllBufferData();
		
		vsTemp->SetShader();
		psTemp->SetShader();

		mrTemp->Draw(_dxContext);
	}
}

RenderManager::RenderManager() {
	mrUID = 0;
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
