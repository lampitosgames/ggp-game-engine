#ifndef GGP_LIGHT_H
#define GGP_LIGHT_H

#include <DirectXMath.h>

struct DirLightStruct {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 direction;
	float diffuseIntensity;
	float ambientIntensity;
	DirectX::XMFLOAT3 empty;
};

struct PointLightStruct {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 position;
	float intensity;
	float range;
	DirectX::XMFLOAT3 empty;
};

#endif //GGP_LIGHT_H