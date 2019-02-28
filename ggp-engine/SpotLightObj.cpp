#include "stdafx.h"

#include "SpotLightObj.h"
#include "ResourceManager.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "SpotLight.h"

using namespace std;
using namespace DirectX::SimpleMath;

Vector3 SpotLightObj::GetEulerFromDir(Vector3 _dir) {
	_dir.Normalize();
	Vector3 zRotAxis = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 yRotAxis = _dir * -1.0f;
	Vector3 xRotAxis = yRotAxis.Cross(zRotAxis);

	Matrix rotMat = Matrix(xRotAxis, yRotAxis, zRotAxis);

	//TODO: Remove all of this when we redo transforms.
	//https://gamedev.stackexchange.com/questions/50963/how-to-extract-euler-angles-from-transformation-matrix
	float pitchX, yawY, rollZ;
	if (rotMat._11 == 1.0f || rotMat._11 == -1.0f) {
		yawY = atan2f(rotMat._13, rotMat._34);
		pitchX = 0.0f;
		rollZ = 0.0f;
	}
	else {
		yawY = atan2(-rotMat._34, rotMat._11);
		pitchX = asin(rotMat._21);
		rollZ = atan2(-rotMat._23, rotMat._22);
	}

	return Vector3(pitchX, yawY, rollZ);
}

SpotLightObj::SpotLightObj(
	ResName _uniqueID,
	Vector3 _position,
	Color _color,
	float _intensity,
	Vector3 _direction,
	float _range,
	float _cutoffAngle,
	float _falloffRate
	) : GameObject(
		_uniqueID == "NA" ? MakeColorUIDString(_color) : _uniqueID, 
		_position, 
		GetEulerFromDir(_direction), 
		Vector3(1.0f, 30.0f / _cutoffAngle, 1.0f)
	) {
	this->type = GOType::SPOT_LIGHT;
	this->color = _color;
	this->intensity = _intensity;
	this->range = _range;
	this->cutoffAngle = _cutoffAngle;
	this->falloffRate = _falloffRate;
	//Direction is always the negated local y axis
	Matrix rotMat = this->transform.GetRotationMatrix();
	this->prevEulerAngles = this->transform.rotation;
	this->direction = -1.0f * Vector3(rotMat._21, rotMat._22, rotMat._23);
}

void SpotLightObj::Start() {
	//Create a material that is the same color as the light
	Material* lightColorMat = resourceManager->AddMaterial(MakeColorUIDString(this->color), res["shdr"]["vert"]["default"], res["shdr"]["pix"]["debug"], this->color);
	//Spotlights are represented by a cone
	Mesh* coneMesh = resourceManager->GetMesh(res["mesh"]["cone"]);
	//Create a mesh renderer
	MeshRenderer* lightMR = this->AddComponent<MeshRenderer>(this);
	lightMR->SetMaterial(lightColorMat);
	lightMR->SetMesh(coneMesh);
	this->AddComponent<SpotLight>(this, this->color, this->transform.position, this->intensity, this->direction, this->range, this->cutoffAngle, this->falloffRate);
}

void SpotLightObj::Update(float _deltaTime) {
	if (this->transform.rotation != this->prevEulerAngles) {
		Matrix rotMat = this->transform.GetRotationMatrix();
		this->prevEulerAngles = this->transform.rotation;
		this->direction = -1.0f * Vector3(rotMat._21, rotMat._22, rotMat._23);
		this->GetComponentType<SpotLight>()->SetDirection(this->direction);
	}
}

DirectX::SimpleMath::Color SpotLightObj::GetColor() { return this->color; }

void SpotLightObj::SetColor(Color _newCol) {
	this->color = _newCol;
	this->GetComponentType<MeshRenderer>()->GetMaterial()->SetColor(_newCol);
	this->GetComponentType<SpotLight>()->SetColor(_newCol);
}

float SpotLightObj::GetIntensity() { return this->intensity; }

void SpotLightObj::SetIntensity(float _newIntensity) {
	this->intensity = _newIntensity;
	this->GetComponentType<SpotLight>()->SetIntensity(_newIntensity);
}

Vector3 SpotLightObj::GetDirection() { return this->direction; }

void SpotLightObj::SetDirection(Vector3 _newDirection) {
	this->direction = _newDirection;
	this->transform.rotation = GetEulerFromDir(this->direction);
	this->prevEulerAngles = this->transform.rotation;
	this->GetComponentType<SpotLight>()->SetDirection(_newDirection);
}

float SpotLightObj::GetRange() { return this->range; }

void SpotLightObj::SetRange(float _newRange) {
	this->range = _newRange;
	this->GetComponentType<SpotLight>()->SetRange(_newRange);
}

float SpotLightObj::GetCutoffAngle() { return this->cutoffAngle; }

void SpotLightObj::SetCutoffAngle(float _newCutoffAngle) {
	this->cutoffAngle = _newCutoffAngle;
	this->GetComponentType<SpotLight>()->SetCutoffAngle(_newCutoffAngle);
}

float SpotLightObj::GetFalloffRate() { return this->falloffRate; }

void SpotLightObj::SetFalloffRate(float _newFalloff) {
	this->falloffRate = _newFalloff;
	this->GetComponentType<SpotLight>()->SetFalloffRate(_newFalloff);
}
