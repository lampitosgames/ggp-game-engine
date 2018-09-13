#ifndef GGP_TRANSFORM_H
#define GGP_TRANSFORM_H

#include <DirectXMath.h>

//struct Basis {
//	DirectX::XMFLOAT3X3 m = DirectX::XMFLOAT3X3();
//
//	//Default constructor stores identity matrix
//	Basis() { DirectX::XMStoreFloat3x3(&m, DirectX::XMMatrixIdentity()); }
//
//	Basis(const DirectX::XMFLOAT3 &xAxis, DirectX::XMFLOAT3 &yAxis, DirectX::XMFLOAT3 &zAxis) {
//
//	}
//
//	//Overload access operators. Can't be used to set the individual axes
//	DirectX::XMFLOAT3 &operator[](int axisIndex) const {
//		//Get the specified axis vector for this local space
//		return DirectX::XMFLOAT3(m(axisIndex, 0), m(axisIndex, 1), m(axisIndex, 2));
//	}
//};

class Transform {
public:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	Transform(DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			  DirectX::XMFLOAT3 _rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			  DirectX::XMFLOAT3 _scale    = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	DirectX::XMFLOAT4X4 GetWorldMatrix();
};

#endif //GGP_TRANSFORM_H