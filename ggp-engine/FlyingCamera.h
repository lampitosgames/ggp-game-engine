#ifndef GAME_FLYING_CAMERA_H
#define GAME_FLYING_CAMERA_H

#include <DirectXMath.h>
#include <string>
#include "Camera.h"
struct InputEvent;

class FlyingCamera : public Camera {
	//Hard-coded movement values
	float camSpeed = 3.0f;
	float horSensitivity = 0.001f;
	float vertSensitivity = 0.001f;

public:
	//Dummy constructor.  Simply passes values to parent
	FlyingCamera(std::string _uniqueID = "NA",
				 float _fov = 1.3962634f,
				 float _aspectRatio = 1.777777777f,
				 float _nearPlane = 0.01f,
				 float _farPlane = 1000.0f,
				 DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
				 DirectX::XMFLOAT3 _rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
				 DirectX::XMFLOAT3 _scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	void Update(float _deltaTime);

	void Input(InputEvent _event);
};

#endif //GAME_FLYING_CAMERA_H