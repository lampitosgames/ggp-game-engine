#ifndef GGP_CAMERA_H
#define GGP_CAMERA_H

#include "GameObject.h"

class Camera : public GameObject {
protected:
	float fov = 1.3962634f; //80 degree FOV
	float aspectRatio = 1.777777777f; //1920 / 1080
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;

	//View and Projection Matrices
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projMatrix;
public:
	//Constructor. Passes transform data to parent.
	//FOV is in radians
	Camera(std::string _uniqueID = "NA",
		   float _fov = 1.3962634f,
		   float _aspectRatio = 1.777777777f,
		   float _nearPlane = 0.01f,
		   float _farPlane = 1000.0f,
		   DirectX::SimpleMath::Vector3 _position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		   DirectX::SimpleMath::Vector3 _rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		   DirectX::SimpleMath::Vector3 _scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

	//Build view matrix from member data and return it
	void CalculateViewMatrix();
	//Build projection matrix from member data and return it
	void CalculateProjectionMatrix();

	//Get/Set the fov angle (in radians)
	float GetFOV();
	void SetFOV(float _fovAngle);
	//Get/Set the fov angle (in degrees)
	float GetFOVDegrees();
	void SetFOVDegrees(float _fovAngle);
	//Get/Set the aspect ratio
	float GetAspectRatio();
	void SetAspectRatio(float _aspectRatio);
	//Get/Set near plane
	float GetNear();
	void SetNear(float _near);
	//Get/Set far plane
	float GetFar();
	void SetFar(float _far);

	//Get the view matrix
	DirectX::SimpleMath::Matrix GetViewMatrix();
	//Get the projection matrix
	DirectX::SimpleMath::Matrix GetProjectionMatrix();
};

#endif //GGP_CAMERA_H