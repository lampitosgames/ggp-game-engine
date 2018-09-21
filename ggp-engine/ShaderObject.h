#ifndef GGP_SHADER_OBJECT_H
#define GGP_SHADER_OBJECT_H

#include "Spatial.h"
#include <d3d11.h>

class ShaderObject : public Spatial {
public:
	//Purely a pass-upward constructor
	ShaderObject(std::string _uniqueID, DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scale) : Spatial(_uniqueID, _position, _rotation, _scale) {};
	virtual void uploadShaderVariables(ID3D11DeviceContext* _dxContext) = 0;
};

#endif //GGP_SHADER_OBJECT_H