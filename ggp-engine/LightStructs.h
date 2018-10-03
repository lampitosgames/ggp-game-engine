#ifndef GGP_LIGHT_H
#define GGP_LIGHT_H

#include <DirectXMath.h>

struct DirLightStruct {
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 direction;
	float emptyPadding;
};

struct PointLightStruct {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 position;
	float ambientColor;
	float diffuseColor;
	// L = (AmbientColor + diffuseColor)/(constAtten + linearAtten*dist + expAtten*dist*dist)
	float constAtten;
	float linearAtten;
	float expAtten;
};

#endif //GGP_LIGHT_H