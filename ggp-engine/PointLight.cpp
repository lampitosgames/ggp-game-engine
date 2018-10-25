#include "PointLight.h"
#include "Spatial.h"
using namespace DirectX;

PointLight::PointLight(UINT _uniqueID, Spatial* _gameObject) {
	//Call the fully parameterized constructor
	PointLight(_uniqueID, _gameObject, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

PointLight::PointLight(UINT _uniqueID, Spatial* _gameObject, XMFLOAT4 _color, float _intensity, float _constAtten, float _linearAtten, float _expAtten) {
	type = CompType::POINT_LIGHT;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	//Store light data
	lightData = {
		_color,
		gameObject->transform.position,
		_intensity,
		_constAtten,
		_linearAtten,
		_expAtten
	};
}

PointLight::~PointLight() {}

DirectX::XMFLOAT4 PointLight::GetColor() { return lightData.color; }

void PointLight::SetColor(DirectX::XMFLOAT4 _color) { lightData.color = _color; }

float PointLight::GetIntensity() { return lightData.intensity; }

void PointLight::SetIntensity(float _newIntensity) { lightData.intensity = _newIntensity; }

float PointLight::GetConstAtten() { return lightData.constAtten; }

float PointLight::GetLinearAtten() { return lightData.linearAtten; }

float PointLight::GetExpAtten() { return lightData.expAtten; }

void PointLight::SetConstAtten(float _constAtten) { lightData.constAtten = _constAtten; }

void PointLight::SetLinearAtten(float _linearAtten) { lightData.linearAtten = _linearAtten; }

void PointLight::SetExpAtten(float _expAtten) { lightData.expAtten = _expAtten; }

PointLightStruct PointLight::buildLightStruct() {
	lightData.position = gameObject->transform.position;
	return lightData;
}
