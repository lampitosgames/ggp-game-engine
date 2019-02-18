#ifndef GGP_TRANSFORM_H
#define GGP_TRANSFORM_H

#include <SimpleMath.h>

class Transform {
public:
	Transform* parent;

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
	DirectX::SimpleMath::Vector3 scale;

	Transform(DirectX::SimpleMath::Vector3 _position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
			  DirectX::SimpleMath::Vector3 _rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
			  DirectX::SimpleMath::Vector3 _scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

	DirectX::SimpleMath::Vector3 Up();
	DirectX::SimpleMath::Vector3 Forward();
	DirectX::SimpleMath::Vector3 Right();

	DirectX::SimpleMath::Matrix GetWorldMatrix(bool _transposed = true);
	DirectX::SimpleMath::Matrix GetRotationMatrix();
	DirectX::SimpleMath::Matrix GetWorldInvTransMatrix();
private:
	//Rotates a given vector by this transform's rotation matrix
	DirectX::SimpleMath::Vector3 XFormVector(DirectX::SimpleMath::Vector3 _vector);
};

#endif //GGP_TRANSFORM_H