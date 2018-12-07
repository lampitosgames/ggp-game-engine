#include "stdafx.h"

#include "FlyingCamera.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "InputListener.h"
#include <algorithm>

using namespace std;
using namespace DirectX;

//Dummy constructor that passes things to the parent camera
FlyingCamera::FlyingCamera(string _uniqueID,
						   float _fov,
						   float _aspectRatio,
						   float _nearPlane,
						   float _farPlane,
						   XMFLOAT3 _position,
						   XMFLOAT3 _rotation,
						   XMFLOAT3 _scale) : Camera(_uniqueID, _fov, _aspectRatio, _nearPlane, _farPlane, _position, _rotation, _scale) {
	this->AddComponent<InputListener>(this);
}

void FlyingCamera::Update(float _deltaTime) {
	//Get transform position and rotation
	XMFLOAT3 fPosition = transform.position;
	//Get axis vectors
	XMFLOAT3 fForward = transform.Forward();
	XMFLOAT3 fRight = transform.Right();
	//Vertical movement is done in global space to help prevent disorientation
	XMFLOAT3 fUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

	//Translation vector that will store the sum of all movement directions
	XMVECTOR translate = XMVectorZero();

	//Forward
	if (inputManager->ActionPressed("move_forward")) {
		//Load transform forward vector into a directx vector
		XMVECTOR forward = XMLoadFloat3(&fForward);
		translate += forward;
	}
	//Back
	if (inputManager->ActionPressed("move_back")) {
		//Load transform forward vector into a directx vector
		XMVECTOR forward = XMLoadFloat3(&fForward);
		translate -= forward;
	}
	//Right
	if (inputManager->ActionPressed("move_right")) {
		//Load transform right vector into a directx vector
		XMVECTOR right = XMLoadFloat3(&fRight);
		translate += right;
	}
	//Left
	if (inputManager->ActionPressed("move_left")) {
		//Load transform right vector into a directx vector
		XMVECTOR right = XMLoadFloat3(&fRight);
		translate -= right;
	}
	//Up (Moves in global space)
	if (inputManager->ActionPressed("move_up")) {
		XMVECTOR up = XMLoadFloat3(&fUp);
		translate += up;
	}
	//Down (Moves in global space)
	if (inputManager->ActionPressed("move_down")) {
		XMVECTOR up = XMLoadFloat3(&fUp);
		translate -= up;
	}

	//If there is movement
	if (!XMVector3Equal(translate, XMVectorZero())) {
		//Normalize the translation vector (the sum of all active move directions)
		translate = XMVector3Normalize(translate) * _deltaTime * camSpeed;
		//Load the position into a XMVector
		XMVECTOR position = XMLoadFloat3(&fPosition);
		//Store the final position in the transform
		XMStoreFloat3(&transform.position, position + translate);
		//Update the view matrix
		CalculateViewMatrix();
	}
}

void FlyingCamera::Input(InputEvent _event) {
	//Zoom when the event type is scroll
	if (_event.type == InputType::MOUSE_SCROLL) {
		SetFOVDegrees(GetFOVDegrees() - _event.mDelta);
	}
	if (inputManager->ActionReleased("toggle_mouse_lock", _event)) {
		inputManager->SetMouseLocked(false);
		std::cout << "test" << std::endl;
	}
	//If this is a mouse move event
	if (_event.type == InputType::MOUSE_MOVE) {
		XMFLOAT3 fRotation = transform.rotation;
		transform.rotation.y += _event.mMoveX * horSensitivity;
		transform.rotation.x += _event.mMoveY * vertSensitivity;
		CalculateViewMatrix();
	}
}
