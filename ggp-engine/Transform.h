#ifndef GGP_TRANSFORM_H
#define GGP_TRANSFORM_H

#include <DirectXMath.h>

class Transform {
public:
	Transform * parent;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	Transform(DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			  DirectX::XMFLOAT3 _rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			  DirectX::XMFLOAT3 _scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	DirectX::XMFLOAT3 Up();
	DirectX::XMFLOAT3 Forward();
	DirectX::XMFLOAT3 Right();

	DirectX::XMFLOAT4X4 GetWorldMatrix(bool _transposed = true);
	DirectX::XMFLOAT4X4 GetRotationMatrix();
	DirectX::XMFLOAT4X4 GetWorldInvTransMatrix();
private:
	//Rotates a given vector by this transform's rotation matrix
	DirectX::XMFLOAT3 XFormVector(DirectX::XMFLOAT3 _vector);
};

#endif //GGP_TRANSFORM_H