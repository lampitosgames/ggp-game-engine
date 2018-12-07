#ifndef GGP_MATERIAL_H
#define GGP_MATERIAL_H

#include <string>
#include <DirectXMath.h>
#include "SimpleShader.h"
class Texture;

class Material {
protected:
	std::string uniqueID;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	DirectX::XMFLOAT4 baseColor;
	float baseSpecular;
	Texture* diffuse;
	Texture* normal;
	Texture* specular;

	//Protected constructor that simply sets everything to null. Used for the PBRMaterial
	Material();
public:
	//Constructors
	//Blank material constructor
	Material(std::string _uniqueID);
	//Fully-parameterized constructor
	Material(std::string _uniqueID,
			 SimpleVertexShader* _vertexShader,
			 SimplePixelShader* _pixelShader,
			 DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			 float _specular = 0.0f);
	//Texture-only constructor
	Material(std::string _uniqueID,
			 SimpleVertexShader* _vertexShader,
			 SimplePixelShader* _pixelShader,
			 Texture* _texture,
			 float _specular = 0.0f);
	//Diffuse, normal, specular texture resources
	Material(std::string _uniqueID,
			 SimpleVertexShader* _vertexShader,
			 SimplePixelShader* _pixelShader,
			 Texture* _diffuse,
			 Texture* _normal,
			 Texture* _specular);
	~Material();

	//Shader setters
	void SetVertexShader(SimpleVertexShader* _vertexShader);
	void SetPixelShader(SimplePixelShader* _pixelShader);
	//Shader getters
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();

	//Upload material to shaders
	//If _pixelShader isn't passed in, the material will use its pixel shader
	virtual void UploadPSData(DirectX::XMFLOAT3 _cameraPos, ID3D11SamplerState* _samplerState, SimplePixelShader* _pixelShader = nullptr);

	//Color get/set
	DirectX::XMFLOAT4 GetColor();
	void SetColor(DirectX::XMFLOAT4 _newColor);

	//Base specular get/set
	float GetBaseSpecular();
	void SetBaseSpecular(float _newSpecular);

	//Texture get/set
	Texture* GetTexture();
	ID3D11ShaderResourceView* GetTexSRV();
	void SetTexture(Texture* _newTexture);
	bool HasDiffuseTexture();

	//Normal map get/set
	Texture* GetNormalMap();
	ID3D11ShaderResourceView* GetNormalSRV();
	void SetNormalMap(Texture* _newNormal);
	bool HasNormalMap();

	//Specular map get/set
	Texture* GetSpecularMap();
	ID3D11ShaderResourceView* GetSpecularSRV();
	void SetSpecularMap(Texture* _newSpecular);
	bool HasSpecularMap();
};

#endif //GGP_MATERIAL_H