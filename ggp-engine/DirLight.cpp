#include "DirLight.h"

using namespace DirectX;

DirLight::DirLight(UINT _uniqueID, GameObject* _gameObject, XMFLOAT4 _ambientColor, XMFLOAT4 _diffuseColor, XMFLOAT3 _direction) {
	type = CompType::DIRECTIONAL_LIGHT;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	lightData = {
		_ambientColor,
		_diffuseColor,
		_direction,
		0.0f
	};
}

DirLight::~DirLight() {}

DirLightStruct DirLight::buildLightStruct() {
	return lightData;
}
