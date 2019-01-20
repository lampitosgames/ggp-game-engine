#include "stdafx.h"

#include "LightManager.h"
#include "DirLight.h"
#include "GameObject.h"

using namespace DirectX;

DirLight::DirLight(GameObject* _gameObject, XMFLOAT4 _color, XMFLOAT3 _direction, float _diffuseIntensity, float _ambientIntensity) {
	gameObject = _gameObject;
	owner = _gameObject->GetUniqueID();
	lightData = {
		_color,
		_direction,
		_diffuseIntensity,
		_ambientIntensity,
		XMFLOAT3()
	};
	LightManager::GetInstance()->AddDirLight(this);
}

DirLight::~DirLight() {
	LightManager::GetInstance()->RemoveDirLight(this);
}

DirLightStruct DirLight::buildLightStruct() {
	return lightData;
}
