#ifndef GGP_PBR_MATERIAL_H
#define GGP_PBR_MATERIAL_H

#include "Material.h"

class PBRMaterial : public Material {
	float baseRoughness;
	float baseMetalness;
	Texture* roughness;
	Texture* metalness;
public:
	//Blank material constructor. Useful for manually putting all textures into place
	//Doesn't call base constructor since PBR materials need to be set up differently
	PBRMaterial(std::string _uniqueID);
	//Raw value constructor (no texture resources)
	PBRMaterial(std::string _uniqueID,
				SimpleVertexShader* _vertexShader,
				SimplePixelShader* _pixelShader,
				DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
				float _roughness = 0.0f,
				float _metalness = 0.0f);
	//Full texture constructor. Requires all texture channels
	PBRMaterial(std::string _uniqueID,
				SimpleVertexShader* _vertexShader,
				SimplePixelShader* _pixelShader,
				Texture* _albedo,
				Texture* _normal,
				Texture* _roughness,
				Texture* _metalness);

	virtual void UploadPSData(DirectX::XMFLOAT3 _cameraPos, ID3D11SamplerState* _samplerState, SimplePixelShader* _pixelShader = nullptr);

	//Roughness map get/set
	Texture* GetRoughnessMap();
	ID3D11ShaderResourceView* GetRoughnessSRV();
	void SetRoughnessMap(Texture* _newRoughness);
	bool HasRoughnessMap();
	//Base roughness get/set
	float GetBaseRoughness();
	void SetBaseRoughness(float _newRoughness);

	//Metalness map get/set
	Texture* GetMetalnessMap();
	ID3D11ShaderResourceView* GetMetalnessSRV();
	void SetMetalnessMap(Texture* _newMetalness);
	bool HasMetalnessMap();
	//Base metalness get/set
	float GetBaseMetalness();
	void SetBaseMetalness(float _newMetalness);
};

#endif //GGP_PBR_MATERIAL_H