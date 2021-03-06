#include "stdafx.h"

#include "RenderManager.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "SystemManager.h"
#include "ParticleManager.h"
#include "LightManager.h"
#include "Texture.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"

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
	//Reference to the resource json map
	nlohmann::json res = ResourceManager::GetRes();
	//Load default shaders
	defaultVertexShader = resourceManager->GetVertexShader(res["shdr"]["vert"]["default"]);
	defaultPixelShader = resourceManager->GetPixelShader(res["shdr"]["pix"]["phong"]);
	//Build the texture sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//Create the sampler state
	HRESULT samplerSuccessfulLoad = systemManager->GetDevice()->CreateSamplerState(&samplerDesc, &samplerState);
	if (samplerSuccessfulLoad != S_OK) {
		std::cout << "Sampler load error!" << std::endl;
	}

	// Load skybox options ----------------------------------------
	skyboxMesh = resourceManager->GetMesh(res["mesh"]["cube"]);
	skyboxVS = resourceManager->GetVertexShader(res["shdr"]["vert"]["skybox"]);
	skyboxPS = resourceManager->GetPixelShader(res["shdr"]["pix"]["skybox"]);
	skyboxSrv = resourceManager->LoadSRV_DDS(res["tex"]["nightSky"]);

	D3D11_RASTERIZER_DESC rs = {};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.CullMode = D3D11_CULL_FRONT;
	rs.DepthClipEnable = true;
	systemManager->GetDevice()->CreateRasterizerState(&rs, &skyRastState);

	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	systemManager->GetDevice()->CreateDepthStencilState(&ds, &skyDepthState);
}

UINT RenderManager::AddMeshRenderer(MeshRenderer * _meshRenderer) {
	meshRendererUIDMap[mrCount] = _meshRenderer;
	mrCount++;
	return mrCount - 1;
}

MeshRenderer* RenderManager::GetMeshRenderer(MeshRendererID _uniqueID) {
	auto thisMR = meshRendererUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisMR != meshRendererUIDMap.end()) {
		return thisMR->second;
	}
	return nullptr;
}

void RenderManager::RemoveMeshRenderer(MeshRenderer * _meshRenderer) {
	auto mrIt = meshRendererUIDMap.begin();
	for (; mrIt != meshRendererUIDMap.end(); ++mrIt) {
		if (mrIt->second == _meshRenderer) {
			meshRendererUIDMap[mrIt->first] = nullptr;
		}
	}
}

void RenderManager::SetSkyboxPS(SimplePixelShader * aSkyPS) {
	assert(aSkyPS != nullptr);
	skyboxPS = aSkyPS;
}

void RenderManager::Render() {
	ID3D11DeviceContext* _dxContext = systemManager->GetContext();
	// Render the skybox ------------------------
	// Set up sky render states
	_dxContext->RSSetState(skyRastState);
	_dxContext->OMSetDepthStencilState(skyDepthState, 0);

	// After drawing all of our regular (solid) objects, draw the sky!
	ID3D11Buffer* skyVB = skyboxMesh->GetVertexBuffer();
	ID3D11Buffer* skyIB = skyboxMesh->GetIndexBuffer();

	// Set the buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_dxContext->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	_dxContext->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);

	// Send the camera's view and proj matrices 
	skyboxVS->SetMatrix4x4("view", activeCamera->GetViewMatrix());
	skyboxVS->SetMatrix4x4("projection", activeCamera->GetProjectionMatrix());

	skyboxVS->CopyAllBufferData();
	skyboxVS->SetShader();

	// Send texture-related stuff
	skyboxPS->SetShaderResourceView("SkyTexture", skyboxSrv);
	skyboxPS->SetSamplerState("BasicSampler", samplerState);

	skyboxPS->CopyAllBufferData(); // Remember to copy to the GPU!!!!
	skyboxPS->SetShader();

	// Reset any changed render states!
	_dxContext->RSSetState(skyRastState);
	_dxContext->OMSetDepthStencilState(skyDepthState, 0);

	// Draw the skybox "mesh" ( a cube )
	_dxContext->DrawIndexed(skyboxMesh->GetIndexCount(), 0, 0);

	// Reset any states we've changed for the next frame!
	_dxContext->RSSetState(0);
	_dxContext->OMSetDepthStencilState(0, 0);


	//Render Objects ---------------------------------
	RenderShadows();
	//Loop through and render every object
	std::map<UINT, MeshRenderer*>::iterator mrIterator;
	for (mrIterator = meshRendererUIDMap.begin(); mrIterator != meshRendererUIDMap.end();) {
		MeshRenderer* mrTemp = mrIterator->second;

		//Make sure the mesh renderer still exists
		if (mrTemp == nullptr) {
			meshRendererUIDMap.erase(mrIterator++);
			continue;
		}
		else {
			mrIterator++;
		}
		//If the mesh renderer doesn't have a mesh to render, continue
		if (mrTemp->GetMesh() == nullptr) { continue; }

		//Get the material's shaders
		SimpleVertexShader* vsTemp = mrTemp->GetVertexShader();
		SimplePixelShader* psTemp = mrTemp->GetPixelShader();
		Material* matTemp = mrTemp->GetMaterial();

		//Null check vertex shader
		if (vsTemp == nullptr) { vsTemp = defaultVertexShader; }
		//Null check pixel shader
		if (psTemp == nullptr) { psTemp = defaultPixelShader; }

		//Upload lighting data from the light manager
		lightManager->UploadAllLights(psTemp);
		//If this object recieves shadows
		//TODO: Fix this
		if (mrTemp->DoesRecieveShadows()) {
			//Upload shadow data from the light manager
			lightManager->UploadAllShadows(vsTemp, psTemp);
		}

		//Upload all data to vertex shader
		vsTemp->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
		vsTemp->SetMatrix4x4("view", activeCamera->GetViewMatrix());
		vsTemp->SetMatrix4x4("projection", activeCamera->GetProjectionMatrix());
		vsTemp->SetMatrix4x4("worldInvTrans", mrTemp->GetWorldInvTransMatrix());
		vsTemp->CopyAllBufferData();

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
	//Render Particles -------------------------------
	ParticleManager::GetInstance()->Render();

	ID3D11ShaderResourceView* nullSRVs[16] = {};
	_dxContext->PSSetShaderResources(0, 16, nullSRVs);
}

void RenderManager::RenderShadows() {
	lightManager->RenderShadows(meshRendererUIDMap);
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

void RenderManager::SetSkyboxVS(SimpleVertexShader * aSkyVS) {
	assert(aSkyVS != nullptr);

	skyboxVS = aSkyVS;
}

RenderManager::RenderManager() {
	mrCount = 0;
	//Gamma correction value should default to 2.2
	gammaCorrection = 2.2f;
	//Get an instance of the resource manager
	systemManager = SystemManager::GetInstance();
	resourceManager = ResourceManager::GetInstance();
	lightManager = LightManager::GetInstance();
}

RenderManager::~RenderManager() {
	//Destructor is the same as clearing the singleton
	Release();
}

void RenderManager::Release() {
	//Reset mesh renderer unique ID values
	mrCount = 0;
	//Clear the map so the singleton can be reused.
	meshRendererUIDMap.clear();

	//Release the sampler state
	samplerState->Release();

	// Cleanup the skybox
	skyboxVS = nullptr;
	skyboxPS = nullptr;
	skyboxMesh = nullptr;
	skyRastState->Release();
	skyDepthState->Release();
	skyboxSrv->Release();
}
