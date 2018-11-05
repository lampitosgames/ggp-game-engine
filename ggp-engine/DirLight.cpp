#include "stdafx.h"

#include "DirLight.h"

using namespace DirectX;

DirLight::DirLight(UINT _uniqueID, GameObject* _gameObject, XMFLOAT4 _color, XMFLOAT3 _direction, float _diffuseIntensity, float _ambientIntensity) {
	type = CompType::DIRECTIONAL_LIGHT;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	lightData = {
		_color,
		_direction,
		_diffuseIntensity,
		_ambientIntensity,
		XMFLOAT3()
	};
}

DirLight::~DirLight() {}

DirLightStruct DirLight::buildLightStruct() {
	return lightData;
}
