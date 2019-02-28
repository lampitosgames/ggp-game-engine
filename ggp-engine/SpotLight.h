#ifndef GGP_SPOT_LIGHT_H
#define GGP_SPOT_LIGHT_H

#include <SimpleMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
class GameObject;

class SpotLight : public BaseComponent<SpotLight> {
	SpotLightStruct lightData;
public:
	SpotLight(GameObject* _gameObject);
	SpotLight(
		GameObject* _gameObject,
		DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::SimpleMath::Vector3 _position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		float _intensity = 1.0f,
		DirectX::SimpleMath::Vector3 _direction = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f),
		float _range = 10.0f,
		float _cutoffAngle = 30.0f,
		float _falloffRate = 10.0f
	);
	~SpotLight();

	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _color);
	DirectX::SimpleMath::Vector3 GetPosition();
	void SetPosition(DirectX::SimpleMath::Vector3 _newPos);
	float GetIntensity();
	void SetIntensity(float _newIntensity);
	DirectX::SimpleMath::Vector3 GetDirection();
	void SetDirection(DirectX::SimpleMath::Vector3 _newDirection);
	float GetRange();
	void SetRange(float _newRange);
	float GetCutoffAngle();
	void SetCutoffAngle(float _newCutoff);
	float GetFalloffRate();
	void SetFalloffRate(float _newFalloffRate);

	SpotLightStruct buildLightStruct();

	GameObject* gameObject;
};

#endif //GGP_SPOT_LIGHT_H