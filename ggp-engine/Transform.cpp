#include "Transform.h"

using namespace DirectX;

Transform::Transform() {
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

XMFLOAT4X4 Transform::GetWorldMatrix() {
	//Create 3 transformation matrices from position data
	XMMATRIX tempTrans = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX tempRot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX tempScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//Create world matrix by combining all 3 transformation matrices
	XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(tempScale, tempRot), tempTrans);
	//Convert back to float matrix
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));
	//Return float matrix
	return worldMatrix;
}
