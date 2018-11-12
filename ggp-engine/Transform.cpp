#include "stdafx.h"

#include "Transform.h"

using namespace DirectX;

Transform::Transform(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _rotation, DirectX::XMFLOAT3 _scale) {
	position = _position;
	rotation = _rotation;
	scale = _scale;
	parent = nullptr;
}

DirectX::XMFLOAT3 Transform::Up() {
	return XFormVector(XMFLOAT3(0.0f, 1.0f, 0.0f));
}

DirectX::XMFLOAT3 Transform::Forward() {
	return XFormVector(XMFLOAT3(0.0f, 0.0f, 1.0f));
}

DirectX::XMFLOAT3 Transform::Right() {
	return XFormVector(XMFLOAT3(1.0f, 0.0f, 0.0f));
}

XMFLOAT4X4 Transform::GetWorldMatrix(bool _transposed) {
	//Include parent in calculation
	XMMATRIX parentWorld = XMMatrixIdentity();
	if (parent != nullptr) {
		XMFLOAT4X4 parentWorldMat = parent->GetWorldMatrix(false);
		parentWorld = XMLoadFloat4x4(&parentWorldMat);
	}
	//Create 3 transformation matrices from position data
	XMMATRIX tempTrans = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX tempRot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX tempScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	//Create world matrix by combining all 3 transformation matrices
	XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(tempScale, tempRot), tempTrans);
	//Multiply by the parent's world matrix
	world = XMMatrixMultiply(world, parentWorld);
	//Convert back to float matrix
	XMFLOAT4X4 worldMatrix;
	if (_transposed) {
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));
	} else {
		XMStoreFloat4x4(&worldMatrix, world);
	}
	//Return float matrix
	return worldMatrix;
}

XMFLOAT4X4 Transform::GetRotationMatrix() {
	//Include parent in calculation
	XMMATRIX parentRot = XMMatrixIdentity();
	if (parent != nullptr) {
		XMFLOAT4X4 parentRotMat = parent->GetRotationMatrix();
		parentRot = XMLoadFloat4x4(&parentRotMat);
	}

	XMFLOAT4X4 rotMatrix;
	//Calculate and store final rotation matrix, incluing parent rotations
	XMStoreFloat4x4(&rotMatrix, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z), parentRot));
	return rotMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInvTransMatrix() {
	//Get the transformation matrix
	XMFLOAT4X4 tempWorld = GetWorldMatrix();
	//Invert and transpose the matrix
	XMMATRIX tempInvTrans = XMMatrixTranspose(XMMatrixInverse(0, XMLoadFloat4x4(&tempWorld)));
	//Store it into an output matrix
	XMFLOAT4X4 worldInvTrans;
	XMStoreFloat4x4(&worldInvTrans, tempInvTrans);
	//Return the result
	return worldInvTrans;
}

DirectX::XMFLOAT3 Transform::XFormVector(DirectX::XMFLOAT3 _vector) {
	//Get initial rotation matrix and up unit vector
	XMFLOAT4X4 tempRotMat = GetRotationMatrix();
	//Store the rotated vector
	XMVECTOR rotatedVector = XMVector3Transform(XMLoadFloat3(&_vector), XMLoadFloat4x4(&tempRotMat));
	//Convert to float3 data type
	XMFLOAT3 finalVector;
	XMStoreFloat3(&finalVector, rotatedVector);
	//Return the final value
	return finalVector;
}
