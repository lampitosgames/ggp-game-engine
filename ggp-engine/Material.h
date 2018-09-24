#ifndef GGP_MATERIAL_H
#define GGP_MATERIAL_H

#include <string>
#include <DirectXMath.h>
#include "SimpleShader.h"

class Material {
	std::string uniqueID;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	DirectX::XMFLOAT4 color;
	//Texture* texture;
public:
	Material(std::string _uniqueID);
	Material(std::string _uniqueID, 
			 SimpleVertexShader* _vertexShader, 
			 SimplePixelShader* _pixelShader, 
			 DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	~Material();

	//Shader setters
	void SetVertexShader(SimpleVertexShader* _vertexShader);
	void SetPixelShader(SimplePixelShader* _pixelShader);
	//Shader getters
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();

	//Color get/set
	DirectX::XMFLOAT4 GetColor();
	void SetColor(DirectX::XMFLOAT4 _newColor);
};

#endif //GGP_MATERIAL_H