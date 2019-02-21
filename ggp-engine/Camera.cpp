#include "stdafx.h"

#include "Camera.h"
#include "InputManager.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

Camera::Camera(ResName _uniqueID,
			   float _fov,
			   float _aspectRatio,
			   float _nearPlane,
			   float _farPlane,
			   Vector3 _position,
			   Vector3 _rotation,
			   Vector3 _scale) : GameObject(_uniqueID, _position, _rotation, _scale) {
	//This is a camera object
	type = GOType::CAMERA;

	//First-time calculate the matrices
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}

void Camera::CalculateViewMatrix() {
	Vector3 forwardV = transform.Forward();
	Vector3 upV = transform.Up();
	viewMatrix = Matrix::CreateLookAt(transform.position, transform.position - forwardV, upV).Transpose();
}

void Camera::CalculateProjectionMatrix() {
	//Using DirectXMath instead of SimpleMath because there is no equivalent perspective left-handed function in SimpleMath
	projMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane));
}

float Camera::GetFOV() { return fov; }

void Camera::SetFOV(float _fovAngle) {
	fov = _fovAngle;
	CalculateProjectionMatrix();
}

float Camera::GetFOVDegrees() { return 57.2958f * fov; }

void Camera::SetFOVDegrees(float _fovAngle) {
	fov = _fovAngle * (3.14159f / 180.0f);
	CalculateProjectionMatrix();
}

float Camera::GetAspectRatio() { return aspectRatio; }

void Camera::SetAspectRatio(float _aspectRatio) {
	aspectRatio = _aspectRatio;
	CalculateProjectionMatrix();
}

float Camera::GetNear() { return nearPlane; }

void Camera::SetNear(float _near) {
	nearPlane = _near;
	CalculateProjectionMatrix();
}

float Camera::GetFar() { return farPlane; }

void Camera::SetFar(float _far) {
	farPlane = _far;
	CalculateProjectionMatrix();
}

Matrix Camera::GetViewMatrix() { return viewMatrix; }

Matrix Camera::GetProjectionMatrix() { return projMatrix; }
