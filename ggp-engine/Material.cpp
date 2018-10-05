#include "Material.h"
#include "Texture.h"

using namespace DirectX;
using namespace std;

Material::Material(std::string _uniqueID) {
	uniqueID = _uniqueID;
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//With the default constructor, the object will use the renderManager's default shaders
	vertexShader = nullptr;
	pixelShader = nullptr;
	texture = nullptr;
}

Material::Material(string _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, XMFLOAT4 _color) {
	uniqueID = _uniqueID;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	color = _color;
}

Material::Material(std::string _uniqueID, SimpleVertexShader * _vertexShader, SimplePixelShader * _pixelShader, Texture * _texture) {
	uniqueID = _uniqueID;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	texture = _texture;
}

Material::~Material() {}

void Material::SetVertexShader(SimpleVertexShader* _vertexShader) { vertexShader = _vertexShader; }

void Material::SetPixelShader(SimplePixelShader* _pixelShader) { pixelShader = _pixelShader; }

SimpleVertexShader* Material::GetVertexShader() { return vertexShader; }

SimplePixelShader* Material::GetPixelShader() { return pixelShader; }

DirectX::XMFLOAT4 Material::GetColor() { return color; }

void Material::SetColor(DirectX::XMFLOAT4 _newColor) { color = _newColor; }

Texture* Material::GetTexture() { return texture; }

ID3D11Resource* Material::GetTexResource() {
	if (texture == nullptr) { return nullptr; }
	return texture->GetTexResource();
}

ID3D11ShaderResourceView* Material::GetTexSRV() {
	if (texture == nullptr) { return nullptr; }
	return texture->GetShaderResourceView();
}

ID3D11SamplerState* Material::GetTexSS() {
	if (texture == nullptr) { return nullptr; }
	return texture->GetSamplerState();
}

void Material::SetTexture(Texture * _newTexture) { texture = _newTexture; }
