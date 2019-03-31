#include "stdafx.h"

#include "LightManager.h"
#include "DirLight.h"
#include "GameObject.h"

using namespace DirectX::SimpleMath;

DirLight::DirLight(GameObject* _gameObject, Color _color, Vector3 _direction, float _diffuseIntensity, float _ambientIntensity) {
	gameObject = _gameObject;
	owner = _gameObject->GetUniqueID();
	lightData = {
		_color,
		_direction,
		_diffuseIntensity,
		_ambientIntensity,
		Vector3()
	};
	LightManager::GetInstance()->AddDirLight(this);
}

DirLight::~DirLight() {
	LightManager::GetInstance()->RemoveDirLight(this);
}

Color DirLight::GetColor() { return lightData.color; }
void DirLight::SetColor(Color _newColor) { lightData.color = _newColor; }
Vector3 DirLight::GetDirection() { return lightData.direction; }
void DirLight::SetDirection(Vector3 _newDir) { lightData.direction = _newDir; }
float DirLight::GetDiffuse() { return lightData.diffuseIntensity; }
void DirLight::SetDiffuse(float _newDI) { lightData.diffuseIntensity = _newDI; }
float DirLight::GetAmbient() { return lightData.ambientIntensity; }
void DirLight::SetAmbient(float _newAI) { lightData.ambientIntensity = _newAI; }

DirLightStruct DirLight::buildLightStruct() {
	return lightData;
}
