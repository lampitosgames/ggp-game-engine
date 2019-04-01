#include "stdafx.h"

#include "LightManager.h"
#include "DXCore.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;

LightManager* LightManager::instance = nullptr;
ID3D11Device* LightManager::dxDevice = nullptr;
ID3D11DeviceContext* LightManager::dxContext = nullptr;

LightManager* LightManager::GetInstance() {
	if (instance == nullptr) {
		instance = new LightManager();
	}
	return instance;
}

void LightManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void LightManager::Start() {
	//Get references to the device and context
	//TODO: Replace resource manager with the system singleton
	dxDevice = ResourceManager::GetDevicePointer();
	dxContext = ResourceManager::GetContextPointer();
	//Create shadow requirements
	//Get the shadow vertex shader
	shadowVS = ResourceManager::GetInstance()->GetVertexShader(ResourceManager::GetRes()["shdr"]["shadow"]["texVS"]);
	//Make shadow map texture
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ID3D11Texture2D* shadowMap;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = shadowMapSize;
	shadowMapDesc.Width = shadowMapSize;
	HRESULT hr = dxDevice->CreateTexture2D(&shadowMapDesc, nullptr, &shadowMap);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow map texture");
	}

	//Depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = dxDevice->CreateDepthStencilView(shadowMap, &dsvDesc, &dirDSV);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow depth stencil view");
	}

	//Shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = dxDevice->CreateShaderResourceView(shadowMap, &srvDesc, &dirSRV);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow SRV");
	}

	//Comparison sampler state
	D3D11_SAMPLER_DESC compSamplerDesc;
	ZeroMemory(&compSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	compSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	compSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	compSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	compSamplerDesc.MinLOD = 0.f;
	compSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	compSamplerDesc.MipLODBias = 0.0f;
	compSamplerDesc.MaxAnisotropy = 0;
	compSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	compSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	compSamplerDesc.BorderColor[0] = 1.0f;
	compSamplerDesc.BorderColor[1] = 1.0f;
	compSamplerDesc.BorderColor[2] = 1.0f;
	compSamplerDesc.BorderColor[3] = 1.0f;
	hr = dxDevice->CreateSamplerState(&compSamplerDesc, &dirCSS);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow comparison sampler state");
	}

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.DepthClipEnable = true;
	rastDesc.DepthBias = 1000;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 1.0f;
	hr = dxDevice->CreateRasterizerState(&rastDesc, &shadowRast);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow rasterizer state");
	}
}

void LightManager::RenderShadows(const std::map<UINT, MeshRenderer*>& _meshes) {
	ID3D11DeviceContext* dxContext = ResourceManager::GetContextPointer();
	//Set up rendering pipeline for depth rendering 2d shadow maps
	dxContext->OMSetRenderTargets(0, 0, dirDSV);
	dxContext->ClearDepthStencilView(dirDSV, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);
	dxContext->RSSetState(shadowRast);
	//Create a viewport for the shadow render target
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)shadowMapSize;
	viewport.Height = (float)shadowMapSize;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dxContext->RSSetViewports(1, &viewport);
	//Get the shader
	shadowVS->SetShader();
	//Upload light data to the shader (this will need to change when we add multiple lights)
	DirectX::XMFLOAT4X4 viewMat = dirLightUIDMap.begin()->second->GetViewMatrix();
	DirectX::XMFLOAT4X4 projMat = dirLightUIDMap.begin()->second->GetProjMatrix();

	shadowVS->SetMatrix4x4("view", viewMat);
	shadowVS->SetMatrix4x4("projection", projMat);

	dxContext->PSSetShader(0, 0, 0);
	//Render all meshes
	for (auto mrIt = _meshes.begin(); mrIt != _meshes.end();) {
		MeshRenderer* mrTemp = mrIt->second;
		//Make sure the mesh renderer still exists
		mrIt++;
		if (mrTemp == nullptr) { continue; }
		//If the mesh renderer doesn't have a mesh to render, continue
		if (mrTemp->GetMesh() == nullptr) { continue; }

		//Copy the mesh's world matrix
		shadowVS->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
		shadowVS->CopyAllBufferData();
		//Draw the mesh to the DSV
		mrTemp->Draw(dxContext);
	}
	//Reset rendering options
	//context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	//viewport.Width = (float)this->width;
	//viewport.Height = (float)this->height;
	//context->RSSetViewports(1, &viewport); // Viewport that matches screen size
	//context->RSSetState(0); // Default rasterizer options
}

void LightManager::UploadAllLights(SimplePixelShader* _pixelShader) {
	//TODO: Only upload lights close to objects
	UINT dirLightCount = 0;
	UINT pointLightCount = 0;
	UINT spotLightCount = 0;
	//Loop through and get every directional light
	std::map<UINT, DirLight*>::iterator dlIt;
	for (dlIt = dirLightUIDMap.begin(); dlIt != dirLightUIDMap.end();) {
		if (dlIt->second == nullptr) {
			dirLightUIDMap.erase(dlIt++);
			continue;
		}
		//Add the directional light to the array
		dirLights[dirLightCount] = dlIt->second->buildLightStruct();
		dirLightCount += 1;
		//Break out of loop if we've hit the max number of lights
		if (dirLightCount >= maxDirLights) { break; }
		dlIt++;
	}
	_pixelShader->SetData("dirLights", &dirLights, sizeof(DirLightStruct) * maxDirLights);
	_pixelShader->SetData("dirLightCount", &dirLightCount, sizeof(UINT));

	//Loop through and get every point light
	std::map<UINT, PointLight*>::iterator plIt;
	for (plIt = pointLightUIDMap.begin(); plIt != pointLightUIDMap.end();) {
		if (plIt->second == nullptr) {
			pointLightUIDMap.erase(plIt++);
			continue;
		}
		pointLights[pointLightCount] = plIt->second->buildLightStruct();
		pointLightCount += 1;
		if (pointLightCount >= maxPointLights) { break; }
		plIt++;
	}
	_pixelShader->SetData("pointLights", &pointLights, sizeof(PointLightStruct) * maxPointLights);
	_pixelShader->SetData("pointLightCount", &pointLightCount, sizeof(UINT));

	//Loop through and get every spot light
	std::map<UINT, SpotLight*>::iterator slIt;
	for (slIt = spotLightUIDMap.begin(); slIt != spotLightUIDMap.end();) {
		if (slIt->second == nullptr) {
			spotLightUIDMap.erase(slIt++);
			continue;
		}
		spotLights[spotLightCount] = slIt->second->buildLightStruct();
		spotLightCount += 1;
		if (spotLightCount >= maxSpotLights) { break; }
		slIt++;
	}
	_pixelShader->SetData("spotLights", &spotLights, sizeof(SpotLightStruct) * maxSpotLights);
	_pixelShader->SetData("spotLightCount", &spotLightCount, sizeof(UINT));
}

UINT LightManager::GetShadowMapResolution() {
	return shadowMapSize;
}

ID3D11RasterizerState* LightManager::GetShadowRasterizer() {
	return shadowRast;
}

SimpleVertexShader* LightManager::GetShadowVertexShader() {
	return shadowVS;
}

ID3D11DepthStencilView* LightManager::GetShadowDSV() {
	return dirDSV;
}

ID3D11ShaderResourceView* LightManager::GetShadowSRV() {
	return dirSRV;
}

ID3D11SamplerState* LightManager::GetShadowSampler() {
	return dirCSS;
}

#pragma region Directional Lights
DirLightID LightManager::AddDirLight(DirLight* _dirLight) {
	dirLightUIDMap[dlCount] = _dirLight;
	dlCount++;
	return dlCount - 1;
}

DirLight* LightManager::GetDirLight(DirLightID _uniqueID) {
	auto thisDL = dirLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisDL != dirLightUIDMap.end()) {
		return thisDL->second;
	}
	return nullptr;
}

void LightManager::RemoveDirLight(DirLight * _dirLight) {
	auto dlIt = dirLightUIDMap.begin();
	for (; dlIt != dirLightUIDMap.end(); ++dlIt) {
		if (dlIt->second == _dirLight) {
			dirLightUIDMap[dlIt->first] = nullptr;
		}
	}
}

DirLightStruct LightManager::GetDirLightStruct(DirLightID _uniqueID) {
	DirLight* tempDL = GetDirLight(_uniqueID);
	if (tempDL != nullptr) {
		return tempDL->buildLightStruct();
	}
	return {};
}
#pragma endregion

#pragma region Point Lights

PointLightID LightManager::AddPointLight(PointLight* _pointLight) {
	pointLightUIDMap[plCount] = _pointLight;
	plCount++;
	return plCount - 1;
}

PointLight* LightManager::GetPointLight(PointLightID _uniqueID) {
	auto thisPL = pointLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisPL != pointLightUIDMap.end()) {
		return thisPL->second;
	}
	return nullptr;
}

void LightManager::RemovePointLight(PointLight * _pointLight) {
	auto plIt = pointLightUIDMap.begin();
	for (; plIt != pointLightUIDMap.end(); ++plIt) {
		if (plIt->second == _pointLight) {
			pointLightUIDMap[plIt->first] = nullptr;
		}
	}
}

PointLightStruct LightManager::GetPointLightStruct(PointLightID _uniqueID) {
	PointLight* tempPL = GetPointLight(_uniqueID);
	if (tempPL != nullptr) {
		return tempPL->buildLightStruct();
	}
	return {};
}
#pragma endregion

#pragma region Spot Lights
SpotLightID LightManager::AddSpotLight(SpotLight* _spotLight) {
	spotLightUIDMap[slCount] = _spotLight;
	slCount++;
	return slCount - 1;
}

void LightManager::RemoveSpotLight(SpotLight * _spotLight) {
	auto slIt = spotLightUIDMap.begin();
	for (; slIt != spotLightUIDMap.end(); ++slIt) {
		if (slIt->second == _spotLight) {
			spotLightUIDMap[slIt->first] = nullptr;
		}
	}
}

SpotLight* LightManager::GetSpotLight(PointLightID _uniqueID) {
	auto thisSL = spotLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisSL != spotLightUIDMap.end()) {
		return thisSL->second;
	}
	return nullptr;
}

SpotLightStruct LightManager::GetSpotLightStruct(PointLightID _uniqueID) {
	SpotLight* tempSpotL = GetSpotLight(_uniqueID);
	if (tempSpotL != nullptr) {
		return tempSpotL->buildLightStruct();
	}
	return {};
}
#pragma endregion

LightManager::LightManager() {}

LightManager::~LightManager() {
	Release();
}

void LightManager::Release() {
	//Reset directional light unique ID values
	dlCount = 0;
	//Clear the map so the singleton can be reused.
	dirLightUIDMap.clear();
	//Reset point light unique ID values
	plCount = 0;
	//Clear the map so the singleton can be reused.
	pointLightUIDMap.clear();
	//Reset spot light unique ID values
	slCount = 0;
	//Clear the map so the singleton can be reused.
	spotLightUIDMap.clear();
}
