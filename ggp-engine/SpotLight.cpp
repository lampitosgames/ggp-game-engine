#include "stdafx.h"

#include "LightManager.h"
#include "SpotLight.h"
#include "GameObject.h"
using namespace DirectX::SimpleMath;

SpotLight::SpotLight(GameObject* _gameObject) {
	//call the fully parameterized constructor
	SpotLight(_gameObject, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

SpotLight::SpotLight(
	GameObject* _gameObject,
	Color _color,
	Vector3 _position,
	float _intensity,
	Vector3 _direction,
	float _range,
	float _cutoffAngle,
	float _falloffRate) {
	gameObject = _gameObject;
	owner = _gameObject->GetUniqueID();
	gameObject->transform.position = _position;
	lightData = {
		_color,
		_position,
		_intensity,
		_direction,
		_range,
		min(max(_cutoffAngle, 0.0f), 89.9f),
		min(max(_falloffRate, 0.0f), 100.0f)
	};
	LightManager::GetInstance()->AddSpotLight(this);
}

SpotLight::~SpotLight() {
	LightManager::GetInstance()->RemoveSpotLight(this);
}

Color SpotLight::GetColor() { return lightData.color; }
void SpotLight::SetColor(Color _color) { lightData.color = _color; }
Vector3 SpotLight::GetPosition() { return gameObject->transform.position; }
void SpotLight::SetPosition(Vector3 _newPos) { gameObject->transform.position = _newPos; }
float SpotLight::GetIntensity() { return lightData.intensity; }
void SpotLight::SetIntensity(float _newIntensity) { lightData.intensity = _newIntensity; }
Vector3 SpotLight::GetDirection() { return lightData.direction; }
void SpotLight::SetDirection(Vector3 _newDirection) { lightData.direction = _newDirection; }
float SpotLight::GetRange() { return lightData.range; }
void SpotLight::SetRange(float _newRange) { lightData.range = _newRange; }
float SpotLight::GetCutoffAngle() { return lightData.cutoffAngle; }
void SpotLight::SetCutoffAngle(float _newCutoff) { lightData.cutoffAngle = min(max(_newCutoff, 0.0f), 89.9f); }
float SpotLight::GetFalloffRate() { return lightData.falloffRate; }
void SpotLight::SetFalloffRate(float _newFalloffRate) { lightData.falloffRate = min(max(_newFalloffRate, 0.0f), 100.0f); }

SpotLightStruct SpotLight::buildLightStruct() {
	lightData.position = gameObject->transform.position;
	return lightData;
}
