#include "stdafx.h"

#include "FlyingCamera.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "InputListener.h"
#include <algorithm>

using namespace std;
using namespace DirectX::SimpleMath;

//Dummy constructor that passes things to the parent camera
FlyingCamera::FlyingCamera(ResName _uniqueID,
						   float _fov,
						   float _aspectRatio,
						   float _nearPlane,
						   float _farPlane,
						   Vector3 _position,
						   Vector3 _rotation,
						   Vector3 _scale) : Camera(_uniqueID, _fov, _aspectRatio, _nearPlane, _farPlane, _position, _rotation, _scale) {
	this->AddComponent<InputListener>(this);
}

void FlyingCamera::Update(float _deltaTime) {
	//Get transform position and rotation
	Vector3 fPosition = transform.position;
	//Get axis vectors
	Vector3 fForward = transform.Forward();
	Vector3 fRight = transform.Right();
	//Vertical movement is done in global space to help prevent disorientation
	Vector3 fUp = Vector3(0.0f, 1.0f, 0.0f);

	//Translation vector that will store the sum of all movement directions
	Vector3 translate = Vector3();

	//Forward
	if (inputManager->ActionPressed("move_forward")) {
		//Load transform forward vector into a directx vector
		Vector3 forward = XMLoadFloat3(&fForward);
		translate += forward;
	}
	//Back
	if (inputManager->ActionPressed("move_back")) {
		//Load transform forward vector into a directx vector
		Vector3 forward = XMLoadFloat3(&fForward);
		translate -= forward;
	}
	//Right
	if (inputManager->ActionPressed("move_right")) {
		//Load transform right vector into a directx vector
		Vector3 right = XMLoadFloat3(&fRight);
		translate += right;
	}
	//Left
	if (inputManager->ActionPressed("move_left")) {
		//Load transform right vector into a directx vector
		Vector3 right = XMLoadFloat3(&fRight);
		translate -= right;
	}
	//Up (Moves in global space)
	if (inputManager->ActionPressed("move_up")) {
		Vector3 up = XMLoadFloat3(&fUp);
		translate += up;
	}
	//Down (Moves in global space)
	if (inputManager->ActionPressed("move_down")) {
		Vector3 up = XMLoadFloat3(&fUp);
		translate -= up;
	}

	//If there is movement
	if (translate != Vector3()) {
		translate.Normalize();
		translate *= _deltaTime * camSpeed;
		transform.position = fPosition + translate;
		CalculateViewMatrix();
	}
}

void FlyingCamera::Input(InputEvent _event) {
	//Zoom when the event type is scroll
	if (_event.type == InputType::MOUSE_SCROLL) {
		SetFOVDegrees(GetFOVDegrees() - _event.mDelta);
	}
	if (inputManager->ActionReleased("toggle_mouse_lock", _event)) {
		inputManager->SetMouseLocked(!inputManager->GetMouseLocked());
		std::cout << "test" << std::endl;
	}
	//If this is a mouse move event
	if (_event.type == InputType::MOUSE_MOVE) {
		transform.rotation.y += _event.mMoveX * horSensitivity;
		transform.rotation.x += _event.mMoveY * vertSensitivity;
		CalculateViewMatrix();
	}
}
