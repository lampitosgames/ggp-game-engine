#include "stdafx.h"

#include "LightManager.h"
#include "DXCore.h"
#include "ResourceManager.h"
#include "SystemManager.h"
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

#pragma region Initialization
void LightManager::Start() {
	//Get the device and context from DirectX
	dxDevice = systemManager->GetDevice();
	dxContext = systemManager->GetContext();

	//Get the shadow vertex shader
	shadowVS = ResourceManager::GetInstance()->GetVertexShader(ResourceManager::GetRes()["shdr"]["shadow"]);
	//Create shadow requirements
	InitDirShadowData();
	InitPointShadowData();
	//InitSpotShadowData();

	//Comparison sampler state. Used to sample from individual shadow maps
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
	HRESULT hr = dxDevice->CreateSamplerState(&compSamplerDesc, &samplerState);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow comparison sampler state");
	}

	//Rasterizer state. Essentially tells the renderer that there is no pixel shader for shadows
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

void LightManager::InitDirShadowData() {
	//Directx resources can be used/bound to any of the directional lights at runtime.
	//Always create the max number, then just fill slots with data.
	//Make shadow map texture
	D3D11_TEXTURE2D_DESC dirShadowMapDesc;
	ID3D11Texture2D* dirShadowMapArr;
	ZeroMemory(&dirShadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	dirShadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dirShadowMapDesc.MipLevels = 1;
	dirShadowMapDesc.ArraySize = maxDirLights;
	dirShadowMapDesc.SampleDesc.Count = 1;
	dirShadowMapDesc.SampleDesc.Quality = 0;
	dirShadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	dirShadowMapDesc.Height = shadowMapSize;
	dirShadowMapDesc.Width = shadowMapSize;
	HRESULT hr = dxDevice->CreateTexture2D(&dirShadowMapDesc, nullptr, &dirShadowMapArr);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow map texture");
	}

	//Shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = maxDirLights;
	hr = dxDevice->CreateShaderResourceView(dirShadowMapArr, &srvDesc, &dirSRV);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow SRV");
	}

	//Depth stencil view (one for each light)
	//Init the DSV description
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;
	//Loop through all the dir light slots
	for (UINT i = 0; i < maxDirLights; i++) {
		//Change slot to the correct slice from the array
		dsvDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);
		
		//Create a depth stencil view for this light
		ID3D11DepthStencilView* thisDSV;
		hr = dxDevice->CreateDepthStencilView(dirShadowMapArr, &dsvDesc, &thisDSV);
		if (hr != S_OK) {
			throw std::exception("Failed to create shadow depth stencil view");
		}
		//Store it in the DSV array for dir lights
		dirDepthStencilArr[i] = thisDSV;
	}

	//Release local reference to shadow map texture pointer since we no longer need it in C++
	//From here out, we deal only with the DSV and SRV
	dirShadowMapArr->Release();
}

void LightManager::InitPointShadowData() {
	//Make shadow map texture cube
	ID3D11Texture2D* pointShadowMapArr;
	D3D11_TEXTURE2D_DESC pointShadowMapDesc;
	ZeroMemory(&pointShadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	pointShadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	pointShadowMapDesc.MipLevels = 1;
	pointShadowMapDesc.ArraySize = 6 * maxPointLights;
	pointShadowMapDesc.SampleDesc.Count = 1;
	pointShadowMapDesc.SampleDesc.Quality = 0;
	pointShadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	pointShadowMapDesc.Height = shadowMapSize;
	pointShadowMapDesc.Width = shadowMapSize;
	pointShadowMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	HRESULT hr = dxDevice->CreateTexture2D(&pointShadowMapDesc, nullptr, &pointShadowMapArr);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow map texture");
	}

	//Shader resource view - cube map array
	D3D11_SHADER_RESOURCE_VIEW_DESC pointSRVDesc;
	ZeroMemory(&pointSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	pointSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	pointSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	pointSRVDesc.TextureCubeArray.NumCubes = maxPointLights;
	pointSRVDesc.TextureCubeArray.MipLevels = 1;
	pointSRVDesc.TextureCubeArray.MostDetailedMip = 0;
	pointSRVDesc.TextureCubeArray.First2DArrayFace = 0;
	hr = dxDevice->CreateShaderResourceView(pointShadowMapArr, &pointSRVDesc, &pointSRV);
	if (hr != S_OK) {
		throw std::exception("Failed to create shadow SRV");
	}

	//Depth stencil view (6 for each light, one for each direction on the cube map [+x, -x, +y, -y, +z, -z])
	//Init the DSV description
	D3D11_DEPTH_STENCIL_VIEW_DESC pointDSVDesc;
	ZeroMemory(&pointDSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	pointDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	pointDSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	pointDSVDesc.Texture2DArray.MipSlice = 0;
	pointDSVDesc.Texture2DArray.ArraySize = 1;
	//Loop through all the dir light slots
	for (UINT i = 0; i < maxPointLights * 6; i++) {
		//Change slot to the correct slice from the array
		pointDSVDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 1);

		//Create a depth stencil view for this light
		ID3D11DepthStencilView* thisDSV;
		hr = dxDevice->CreateDepthStencilView(pointShadowMapArr, &pointDSVDesc, &thisDSV);
		if (hr != S_OK) {
			throw std::exception("Failed to create shadow depth stencil view");
		}
		//Store it in the DSV array for dir lights
		pointDepthStencilArr[i] = thisDSV;
	}

	//Release local reference to shadow map texture pointer since we no longer need it in C++
	//From here out, we deal only with the DSV and SRV
	pointShadowMapArr->Release();
}
#pragma endregion

void LightManager::RenderShadows(const std::map<UINT, MeshRenderer*>& _meshes) {
	//Create a viewport for the shadow render target
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)shadowMapSize;
	viewport.Height = (float)shadowMapSize;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dxContext->RSSetViewports(1, &viewport);
	//Set up the shadow rasterizer
	dxContext->RSSetState(shadowRast);
	//Set the vertex shader. There is no pixel shader, so set it to null
	shadowVS->SetShader();
	dxContext->PSSetShader(0, 0, 0);

	//For every directional light
	UINT i = 0;
	for (auto dlIt = dirLightUIDMap.begin(); dlIt != dirLightUIDMap.end(); dlIt++) {
		//Set the render target for this iteration
		dxContext->OMSetRenderTargets(0, 0, dirDepthStencilArr[i]);
		dxContext->ClearDepthStencilView(dirDepthStencilArr[i++], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		//Upload light matrices to the shader
		DirectX::XMFLOAT4X4 viewMat = dlIt->second->GetViewMatrix();
		DirectX::XMFLOAT4X4 projMat = dlIt->second->GetProjMatrix();
		shadowVS->SetMatrix4x4("view", viewMat);
		shadowVS->SetMatrix4x4("projection", projMat);
		//Render all meshes
		for (auto mrIt = _meshes.begin(); mrIt != _meshes.end(); mrIt++) {
			MeshRenderer* mrTemp = mrIt->second;
			//Make sure the mesh renderer still exists
			if (mrTemp == nullptr) { continue; }
			//If the mesh renderer doesn't have a mesh to render, continue
			if (mrTemp->GetMesh() == nullptr) { continue; }
			//If the mesh renderer doesn't cast shadows, continue
			if (!mrTemp->DoesCastShadows()) { continue; }
			//Copy the mesh's world matrix
			shadowVS->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
			shadowVS->CopyAllBufferData();
			//Draw the mesh to the DSV
			mrTemp->Draw(dxContext);
		}
	}

	//For every point light
	UINT j = 0;
	for (auto plIt = pointLightUIDMap.begin(); plIt != pointLightUIDMap.end(); plIt++) {
		Matrix viewMats[6];
		plIt->second->GetViewMatrices(viewMats);
		viewMats[0];
		DirectX::XMFLOAT4X4 projMat = plIt->second->GetProjMatrix();
		//For every face on the shadow cube map
		for (UINT f = 0; f < 6; f++) {
			//Set the render target for this iteration
			dxContext->OMSetRenderTargets(0, 0, pointDepthStencilArr[j+f]);
			dxContext->ClearDepthStencilView(pointDepthStencilArr[j+f], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
			//Upload light matrices to the shader
			shadowVS->SetMatrix4x4("view", viewMats[0]);
			shadowVS->SetMatrix4x4("projection", projMat);
			//Render all meshes
			for (auto mrIt = _meshes.begin(); mrIt != _meshes.end(); mrIt++) {
				MeshRenderer* mrTemp = mrIt->second;
				//Make sure the mesh renderer still exists
				if (mrTemp == nullptr) { continue; }
				//If the mesh renderer doesn't have a mesh to render, continue
				if (mrTemp->GetMesh() == nullptr) { continue; }
				//If the mesh renderer doesn't cast shadows, continue
				if (!mrTemp->DoesCastShadows()) { continue; }

				//Copy the mesh's world matrix
				shadowVS->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
				shadowVS->CopyAllBufferData();
				//Draw the mesh to the DSV
				mrTemp->Draw(dxContext);
			}
		}
		j++;
	}
	//Reset rendering options
	ID3D11RenderTargetView* defaultRTV = systemManager->GetDefaultRTV();
	ID3D11DepthStencilView* defaultDSV = systemManager->GetDefaultDSV();
	dxContext->OMSetRenderTargets(1, &defaultRTV, defaultDSV);
	dxContext->RSSetViewports(1, &systemManager->GetViewport()); // Viewport that matches screen size
	dxContext->RSSetState(0); // Default rasterizer options
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

void LightManager::UploadAllShadows(SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader) {
	//Upload dir light data
	DirectX::XMFLOAT4X4 dirLightProjMats[maxDirLights];
	DirectX::XMFLOAT4X4 dirLightViewMats[maxDirLights];
	//Loop through all dir lights to get their view and projection matrices
	UINT i = 0;
	for (auto dlIt = dirLightUIDMap.begin(); dlIt != dirLightUIDMap.end(); dlIt++) {
		dirLightProjMats[i] = dlIt->second->GetProjMatrix();
		dirLightViewMats[i++] = dlIt->second->GetViewMatrix();
	}
	_vertexShader->SetData("dirLightProj", dirLightProjMats, sizeof(DirectX::XMFLOAT4X4) * maxDirLights);
	_vertexShader->SetData("dirLightView", dirLightViewMats, sizeof(DirectX::XMFLOAT4X4) * maxDirLights);
	_pixelShader->SetShaderResourceView("DirShadowMap", dirSRV);

	//Upload point light data
	_pixelShader->SetShaderResourceView("PointShadowMap", pointSRV);
	//Upload spot light data

	//Upload the shadow sampler
	_pixelShader->SetSamplerState("shadowSampler", samplerState);
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
	return dirDepthStencilArr[0];
}

ID3D11ShaderResourceView* LightManager::GetShadowSRV() {
	return dirSRV;
}

ID3D11SamplerState* LightManager::GetShadowSampler() {
	return samplerState;
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

void LightManager::RemovePointLight(PointLight* _pointLight) {
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

SpotLight* LightManager::GetSpotLight(SpotLightID _uniqueID) {
	auto thisSL = spotLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisSL != spotLightUIDMap.end()) {
		return thisSL->second;
	}
	return nullptr;
}

void LightManager::RemoveSpotLight(SpotLight * _spotLight) {
	auto slIt = spotLightUIDMap.begin();
	for (; slIt != spotLightUIDMap.end(); ++slIt) {
		if (slIt->second == _spotLight) {
			spotLightUIDMap[slIt->first] = nullptr;
		}
	}
}

SpotLightStruct LightManager::GetSpotLightStruct(SpotLightID _uniqueID) {
	SpotLight* tempSL = GetSpotLight(_uniqueID);
	if (tempSL != nullptr) {
		return tempSL->buildLightStruct();
	}
	return {};
}
#pragma endregion

LightManager::LightManager() {
	//Get references to the device and context
	systemManager = SystemManager::GetInstance();
}

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

	shadowRast->Release();
	samplerState->Release();
	dirSRV->Release();
	for (UINT i = 0; i < maxDirLights; i++) {
		dirDepthStencilArr[i]->Release();
	}
	pointSRV->Release();
	for (UINT i = 0; i < maxPointLights * 6; i++) {
		pointDepthStencilArr[i]->Release();
	}
}
