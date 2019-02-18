#include "stdafx.h"

#include "Transform.h"

using namespace DirectX::SimpleMath;

Transform::Transform(Vector3 _position, Vector3 _rotation, Vector3 _scale) {
	position = _position;
	rotation = _rotation;
	scale = _scale;
	parent = nullptr;
}

Vector3 Transform::Up() {
	return XFormVector(Vector3(0.0f, 1.0f, 0.0f));
}

Vector3 Transform::Forward() {
	return XFormVector(Vector3(0.0f, 0.0f, 1.0f));
}

Vector3 Transform::Right() {
	return XFormVector(Vector3(1.0f, 0.0f, 0.0f));
}

Matrix Transform::GetWorldMatrix(bool _transposed) {
	Matrix parentWorld = Matrix();
	if (parent != nullptr) {
		Matrix parentWorldMat = parent->GetWorldMatrix(false);
		parentWorld = XMLoadFloat4x4(&parentWorldMat);
	}
	Matrix tempTrans = Matrix::CreateTranslation(position.x, position.y, position.z);
	Matrix tempRot = Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	Matrix tempScale = Matrix::CreateScale(scale.x, scale.y, scale.z);
	Matrix world = (tempScale * tempRot) * tempTrans;
	world = world * parentWorld;
	if (_transposed) {
		world = world.Transpose();
	}
	return world;
}

Matrix Transform::GetRotationMatrix() {
	Matrix parentRot = Matrix();
	if (parent != nullptr) {
		Matrix parentRot = parent->GetRotationMatrix();
	}
	return Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z) * parentRot;
}

Matrix Transform::GetWorldInvTransMatrix() {
	return GetWorldMatrix().Invert().Transpose();
}

Vector3 Transform::XFormVector(Vector3 _vector) {
	return Vector3::Transform(_vector, GetRotationMatrix());
}
