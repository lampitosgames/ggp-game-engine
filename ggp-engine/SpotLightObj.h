#ifndef GGP_SPOT_LIGHT_OBJ_H
#define GGP_SPOT_LIGHT_OBJ_H

#include "GameObject.h"

class SpotLightObj : public GameObject {
	DirectX::SimpleMath::Color color;
	float intensity;
	DirectX::SimpleMath::Vector3 direction;
	float range;
	float cutoffAngle;
	float falloffRate;

	//Keep track of rotation so we know when we need to update the direction vector
	DirectX::SimpleMath::Vector3 prevEulerAngles;
	DirectX::SimpleMath::Vector3 GetEulerFromDir(DirectX::SimpleMath::Vector3 _dir);
public:
	SpotLightObj(
		ResName _uniqueID = "NA",
		DirectX::SimpleMath::Vector3 _position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),
		float _intensity = 1.0f,
		DirectX::SimpleMath::Vector3 _direction = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f),
		float _range = 10.0f,
		float _cutoffAngle = 30.0f,
		float _falloffRate = 10.0f
	);
	void Start();
	void Update(float _deltaTime);

	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _newCol);
	float GetIntensity();
	void SetIntensity(float _newIntensity);
	DirectX::SimpleMath::Vector3 GetDirection();
	void SetDirection(DirectX::SimpleMath::Vector3 _newDirection);
	float GetRange();
	void SetRange(float _newRange);
	float GetCutoffAngle();
	void SetCutoffAngle(float _newCutoffAngle);
	float GetFalloffRate();
	void SetFalloffRate(float _newFalloff);
};

#endif //GGP_SPOT_LIGHT_OBJ_H