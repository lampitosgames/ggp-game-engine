#include "stdafx.h"

#include "LightManager.h"
#include "DirLight.h"
#include "GameObject.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

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

DirectX::SimpleMath::Matrix DirLight::GetViewMatrix() {
	Vector3 thisPos = gameObject->transform.position - this->GetDirection()*100.0f;
	Vector3 thisDir = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 thisUp = gameObject->transform.Up();
	if (thisUp.y == 1.0f) { thisUp = Vector3(1.0f, 0.0f, 0.0f); }

	return Matrix::CreateLookAt(thisPos, thisDir, thisUp).Transpose();
	//return Matrix::CreateLookAt(Vector3(0.0f, 100.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)).Transpose();
}

DirectX::SimpleMath::Matrix DirLight::GetProjMatrix() {
	return Matrix::CreateOrthographic(32.0f, 32.0f, 0.1f, 200.0f).Transpose();
}

DirLightStruct DirLight::buildLightStruct() {
	return lightData;
}
