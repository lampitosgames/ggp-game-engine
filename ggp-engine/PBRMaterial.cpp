#include "stdafx.h"

#include "PBRMaterial.h"
#include "Texture.h"

using namespace std;
using namespace DirectX;

PBRMaterial::PBRMaterial(string _uniqueID) {
	uniqueID = _uniqueID;
	//We only need to initialize variables unique to PBRMaterial since the others are initialized in Material's default constructor
	baseRoughness = 0.0f;
	baseMetalness = 0.0f;
	roughness = nullptr;
	metalness = nullptr;
}

PBRMaterial::PBRMaterial(string _uniqueID,
						 SimpleVertexShader* _vertexShader,
						 SimplePixelShader* _pixelShader,
						 XMFLOAT4 _color,
						 float _roughness,
						 float _metalness) {
	uniqueID = _uniqueID;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	baseColor = _color;
	baseRoughness = _roughness;
	baseMetalness = _metalness;
	roughness = nullptr;
	metalness = nullptr;
}

PBRMaterial::PBRMaterial(string _uniqueID,
						 SimpleVertexShader* _vertexShader,
						 SimplePixelShader* _pixelShader,
						 Texture* _albedo,
						 Texture* _normal,
						 Texture* _roughness,
						 Texture* _metalness) {
	uniqueID = _uniqueID;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	diffuse = _albedo;
	normal = _normal;
	roughness = _roughness;
	metalness = _metalness;
	baseRoughness = 0.0f;
	baseMetalness = 0.0f;
}

void PBRMaterial::UploadPSData(XMFLOAT3 _cameraPos, ID3D11SamplerState* _samplerState, SimplePixelShader* _pixelShader) {
	//Build texture channel toggle array
	XMINT4 channelToggle = XMINT4(HasDiffuseTexture(),
								  HasNormalMap(),
								  HasRoughnessMap(),
								  HasMetalnessMap());
	//Upload to pixel shader
	_pixelShader->SetData("useAlbedoTex", &channelToggle.x, sizeof(int));
	_pixelShader->SetData("useNormalTex", &channelToggle.y, sizeof(int));
	_pixelShader->SetData("useRoughnessTex", &channelToggle.z, sizeof(int));
	_pixelShader->SetData("useMetalnessTex", &channelToggle.w, sizeof(int));

	//If any textures exist, upload the sampler state
	if (channelToggle.x || channelToggle.y || channelToggle.z || channelToggle.w) {
		_pixelShader->SetSamplerState("basicSampler", _samplerState);
	}
	//Only upload texture resources that exist
	if (channelToggle.x) { _pixelShader->SetShaderResourceView("albedoTexture", GetTexSRV()); }
	if (channelToggle.y) { _pixelShader->SetShaderResourceView("normalMap", GetNormalSRV()); }
	if (channelToggle.z) { _pixelShader->SetShaderResourceView("roughnessMap", GetRoughnessSRV()); }
	if (channelToggle.w) { _pixelShader->SetShaderResourceView("metalnessMap", GetMetalnessSRV()); }

	//Upload material data
	_pixelShader->SetFloat3("cameraPosition", _cameraPos);
	_pixelShader->SetFloat4("baseColor", GetColor());
	_pixelShader->SetFloat("baseRoughness", GetBaseRoughness());
	_pixelShader->SetFloat("baseMetalness", GetBaseMetalness());
}

Texture* PBRMaterial::GetRoughnessMap() { return roughness; }

ID3D11ShaderResourceView* PBRMaterial::GetRoughnessSRV() {
	if (roughness == nullptr) { return nullptr; }
	return roughness->GetShaderResourceView();
}

void PBRMaterial::SetRoughnessMap(Texture* _newRoughness) { roughness = _newRoughness; }

bool PBRMaterial::HasRoughnessMap() { return roughness != nullptr; }

float PBRMaterial::GetBaseRoughness() { return baseRoughness; }

void PBRMaterial::SetBaseRoughness(float _newRoughness) { baseRoughness = _newRoughness; }

Texture* PBRMaterial::GetMetalnessMap() { return metalness; }

ID3D11ShaderResourceView* PBRMaterial::GetMetalnessSRV() {
	if (metalness == nullptr) { return nullptr; }
	return metalness->GetShaderResourceView();
}

void PBRMaterial::SetMetalnessMap(Texture* _newMetalness) { metalness = _newMetalness; }

bool PBRMaterial::HasMetalnessMap() { return metalness != nullptr; }

float PBRMaterial::GetBaseMetalness() { return baseMetalness; }

void PBRMaterial::SetBaseMetalness(float _newMetalness) { baseMetalness = _newMetalness; }
