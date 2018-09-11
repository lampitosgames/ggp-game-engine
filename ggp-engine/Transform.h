#ifndef GGP_TRANSFORM_H
#define GGP_TRANSFORM_H

#include <DirectXMath.h>

using namespace DirectX;

class Transform {
public:
	XMFLOAT3 position = XMFLOAT3();
	XMFLOAT4 rotation = XMFLOAT4();
	XMFLOAT3 scale = XMFLOAT3();

	Transform();
	Transform(XMFLOAT3 _pos, XMFLOAT3 _rot, XMFLOAT3 _scl);

	XMFLOAT4X4 GetWorldMatrix();
};

#endif //GGP_TRANSFORM_H