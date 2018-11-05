#include "stdafx.h"

#include "PointLight.h"
#include "GameObject.h"
using namespace DirectX;

PointLight::PointLight(UINT _uniqueID, GameObject* _gameObject) {
	//Call the fully parameterized constructor
	PointLight(_uniqueID, _gameObject, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

PointLight::PointLight(UINT _uniqueID, GameObject* _gameObject, XMFLOAT4 _color, float _intensity, float _range) {
	type = CompType::POINT_LIGHT;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	//Store light data
	lightData = {
		_color,
		gameObject->transform.position,
		_intensity,
		_range
	};
}

PointLight::~PointLight() {}

DirectX::XMFLOAT4 PointLight::GetColor() { return lightData.color; }

void PointLight::SetColor(DirectX::XMFLOAT4 _color) { lightData.color = _color; }

float PointLight::GetIntensity() { return lightData.intensity; }

void PointLight::SetIntensity(float _newIntensity) { lightData.intensity = _newIntensity; }

float PointLight::GetRange() { return lightData.range; }

void PointLight::SetRange(float _newRange) { lightData.range = _newRange; }

PointLightStruct PointLight::buildLightStruct() {
	lightData.position = gameObject->transform.position;
	return lightData;
}
