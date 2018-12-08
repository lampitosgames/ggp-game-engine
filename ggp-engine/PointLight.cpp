#include "stdafx.h"

#include "PointLight.h"
#include "GameObject.h"
#include "LightManager.h"
using namespace DirectX;

PointLight::PointLight(GameObject* _gameObject) {
	//Call the fully parameterized constructor
	PointLight(_gameObject, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

PointLight::PointLight(GameObject* _gameObject, XMFLOAT4 _color, float _intensity, float _range) {
	gameObject = _gameObject;
	owner = gameObject->GetUniqueID();
	//Store light data
	lightData = {
		_color,
		gameObject->transform.position,
		_intensity,
		_range
	};

	LightManager::GetInstance()->AddPointLight(this);
}

PointLight::~PointLight() {
	LightManager::GetInstance()->RemovePointLight(this);
}

DirectX::XMFLOAT4 PointLight::GetColor() {
	return lightData.color;
}

void PointLight::SetColor(DirectX::XMFLOAT4 _color) { lightData.color = _color; }

float PointLight::GetIntensity() { return lightData.intensity; }

void PointLight::SetIntensity(float _newIntensity) { lightData.intensity = _newIntensity; }

float PointLight::GetRange() { return lightData.range; }

void PointLight::SetRange(float _newRange) { lightData.range = _newRange; }

PointLightStruct PointLight::buildLightStruct() {
	lightData.position = gameObject->transform.position;
	return lightData;
}
