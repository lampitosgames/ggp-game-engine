#ifndef GGP_LIGHT_H
#define GGP_LIGHT_H

#include <DirectXMath.h>

struct DirectionalLight {
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 direction;
};

#endif //GGP_LIGHT_H