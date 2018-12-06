#include "stdafx.h"

#include "Material.h"
#include "Texture.h"

using namespace DirectX;
using namespace std;

#pragma region Constructors
Material::Material(std::string _uniqueID) {
	uniqueID = _uniqueID;
	baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	baseSpecular = 0.0f;
	//With the default constructor, the object will use the renderManager's default shaders
	vertexShader = nullptr;
	pixelShader = nullptr;
	diffuse = nullptr;
	normal = nullptr;
	specular = nullptr;
}

Material::Material(string _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, XMFLOAT4 _color, float _specular) {
	uniqueID = _uniqueID;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	baseColor = _color;
	baseSpecular = _specular;
	diffuse = nullptr;
	normal = nullptr;
	specular = nullptr;
}

Material::Material(std::string _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, Texture* _texture, float _specular) {
	uniqueID = _uniqueID;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	baseSpecular = _specular;
	diffuse = _texture;
	normal = nullptr;
	specular = nullptr;
}

Material::Material(std::string _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, Texture* _diffuse, Texture* _normal, Texture* _specular) {
	uniqueID = _uniqueID;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	baseSpecular = 0.0f;
	diffuse = _diffuse;
	normal = _normal;
	specular = _specular;
}
#pragma endregion

Material::~Material() {}

void Material::SetVertexShader(SimpleVertexShader* _vertexShader) { vertexShader = _vertexShader; }

void Material::SetPixelShader(SimplePixelShader* _pixelShader) { pixelShader = _pixelShader; }

SimpleVertexShader* Material::GetVertexShader() { return vertexShader; }

SimplePixelShader* Material::GetPixelShader() { return pixelShader; }

void Material::UploadPSData(DirectX::XMFLOAT3 _cameraPos, ID3D11SamplerState* _samplerState, SimplePixelShader* _pixelShader) {
	//Build texture channel toggle array
	XMINT3 channelToggle = XMINT3(HasDiffuseTexture(),
								  HasNormalMap(),
								  HasSpecularMap());
	//Upload to pixel shader
	_pixelShader->SetData("useDiffuseTex", &channelToggle.x, sizeof(int));
	_pixelShader->SetData("useNormalTex", &channelToggle.y, sizeof(int));
	_pixelShader->SetData("useSpecularTex", &channelToggle.z, sizeof(int));

	//If any textures exist, upload the sampler state
	if (channelToggle.x || channelToggle.y || channelToggle.z) {
		_pixelShader->SetSamplerState("basicSampler", _samplerState);
	}
	//Only upload texture resources that exist
	if (channelToggle.x) { _pixelShader->SetShaderResourceView("diffuseTexture", GetTexSRV()); }
	if (channelToggle.y) { _pixelShader->SetShaderResourceView("normalMap", GetNormalSRV()); }
	if (channelToggle.z) { _pixelShader->SetShaderResourceView("specularMap", GetSpecularSRV()); }

	//Upload material data
	_pixelShader->SetFloat3("cameraPosition", _cameraPos);
	_pixelShader->SetFloat4("baseColor", GetColor());
	_pixelShader->SetFloat("baseSpec", GetBaseSpecular());
}

DirectX::XMFLOAT4 Material::GetColor() { return baseColor; }

void Material::SetColor(DirectX::XMFLOAT4 _newColor) { baseColor = _newColor; }

float Material::GetBaseSpecular() { return baseSpecular; }

void Material::SetBaseSpecular(float _newSpecular) { baseSpecular = _newSpecular; }

#pragma region Diffuse Texture
Texture* Material::GetTexture() { return diffuse; }

ID3D11ShaderResourceView* Material::GetTexSRV() {
	if (diffuse == nullptr) { return nullptr; }
	return diffuse->GetShaderResourceView();
}

void Material::SetTexture(Texture* _newTexture) { diffuse = _newTexture; }

bool Material::HasDiffuseTexture() { return diffuse != nullptr; }
#pragma endregion

#pragma region Normal Map
Texture* Material::GetNormalMap() { return normal; }

ID3D11ShaderResourceView* Material::GetNormalSRV() {
	if (normal == nullptr) { return nullptr; }
	return normal->GetShaderResourceView();
}

void Material::SetNormalMap(Texture* _newNormal) { normal = _newNormal; }

bool Material::HasNormalMap() { return normal != nullptr; }
#pragma endregion

#pragma region Specular Map
Texture* Material::GetSpecularMap() { return specular; }

ID3D11ShaderResourceView* Material::GetSpecularSRV() {
	if (specular == nullptr) { return nullptr; }
	return specular->GetShaderResourceView();
}

void Material::SetSpecularMap(Texture* _newSpecular) { specular = _newSpecular; }

bool Material::HasSpecularMap() { return specular != nullptr; }

Material::Material() {
	vertexShader = nullptr;
	pixelShader = nullptr;
	baseColor = XMFLOAT4();
	baseSpecular = 0.0f;
	diffuse = nullptr;
	normal = nullptr;
	specular = nullptr;
}
#pragma endregion
