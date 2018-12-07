#include "stdafx.h"

#include "Camera.h"
#include "InputManager.h"

using namespace DirectX;

Camera::Camera(std::string _uniqueID,
			   float _fov,
			   float _aspectRatio,
			   float _nearPlane,
			   float _farPlane,
			   XMFLOAT3 _position,
			   XMFLOAT3 _rotation,
			   XMFLOAT3 _scale) : GameObject(_uniqueID, _position, _rotation, _scale) {
	//This is a camera object
	type = GOType::CAMERA;

	//First-time calculate the matrices
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}

void Camera::CalculateViewMatrix() {
	//TODO: Get this from the camera transform
	XMFLOAT3 forwardV = transform.Forward();
	XMFLOAT3 upV = transform.Up();
	XMMATRIX newViewMatrix = XMMatrixLookToLH(XMLoadFloat3(&transform.position), XMLoadFloat3(&forwardV), XMLoadFloat3(&upV));
	//Don't forget to transpose the matrix!
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(newViewMatrix));
}

void Camera::CalculateProjectionMatrix() {
	XMMATRIX newProjectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
	XMStoreFloat4x4(&projMatrix, XMMatrixTranspose(newProjectionMatrix));
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

DirectX::XMFLOAT4X4 Camera::GetViewMatrix() { return viewMatrix; }

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix() { return projMatrix; }
