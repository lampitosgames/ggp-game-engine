#include "stdafx.h"

#include "DirLightObj.h"
#include "DirLight.h"

using namespace DirectX::SimpleMath;

DirLightObj::DirLightObj(
	ResName _uniqueID,
	Color _color,
	Vector3 _direction,
	float _diffuseIntensity,
	float _ambientIntensity) : GameObject(
		_uniqueID == "NA" ? MakeColorUIDString(_color) : _uniqueID,
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1.0f, 1.0f, 1.0f)
	) {
	this->type = GOType::DIR_LIGHT;
	this->color = _color;
	this->direction = _direction;
	this->diffuseIntensity = _diffuseIntensity;
	this->ambientIntensity = _ambientIntensity;
}

void DirLightObj::Start() {
	this->AddComponent<DirLight>(this, this->color, this->direction, this->diffuseIntensity, this->ambientIntensity);
}

Color DirLightObj::GetColor() { return this->color; }

void DirLightObj::SetColor(Color _newCol) {
	this->color = _newCol;
	this->GetComponentType<DirLight>()->SetColor(_newCol);
}

Vector3 DirLightObj::GetDirection() { return this->direction; }

void DirLightObj::SetDirection(Vector3 _newDir) {
	this->direction = _newDir;
	this->GetComponentType<DirLight>()->SetDirection(_newDir);
}

float DirLightObj::GetDiffuse() { return this->diffuseIntensity; }

void DirLightObj::SetDiffuse(float _newDI) {
	this->diffuseIntensity = _newDI;
	this->GetComponentType<DirLight>()->SetDiffuse(_newDI);
}

float DirLightObj::GetAmbient() { return this->ambientIntensity; }

void DirLightObj::SetAmbient(float _newAI) {
	this->ambientIntensity = _newAI;
	this->GetComponentType<DirLight>()->SetAmbient(_newAI);
}
