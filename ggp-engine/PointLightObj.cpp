#include "stdafx.h"

#include "PointLightObj.h"
#include "Material.h"
#include "Mesh.h"
#include "PointLight.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"

using namespace std;
using namespace DirectX::SimpleMath;

float PointLightObj::rangeScaleMod = 25.0f;

PointLightObj::PointLightObj(
	ResName _uniqueID,
	Vector3 _position,
	Color _color,
	float _intensity,
	float _range) : GameObject(
		_uniqueID == "NA" ? MakeColorUIDString(_color) : _uniqueID, 
		_position, 
		Vector3(0.0f, 0.0f, 0.0f), 
		Vector3(_range / rangeScaleMod, _range / rangeScaleMod, _range / rangeScaleMod)
	) {
	//Set member variables
	this->type = GOType::POINT_LIGHT;
	this->color = _color;
	this->intensity = _intensity;
	this->range = _range;
}

void PointLightObj::Start() {
	//Create a material that is the same color as the light
	Material* lightColorMat = resourceManager->AddMaterial(MakeColorUIDString(this->color), res["shdr"]["vert"]["default"], res["shdr"]["pix"]["debug"], this->color);
	//All lights are represented by spherees
	Mesh* sphereMesh = resourceManager->GetMesh(res["mesh"]["sphere"]);
	//Create a mesh renderer
	MeshRenderer* lightMR = this->AddComponent<MeshRenderer>(this);
	lightMR->SetMaterial(lightColorMat);
	lightMR->SetMesh(sphereMesh);
	this->AddComponent<PointLight>(this, this->color, this->intensity, this->range);
}

Color PointLightObj::GetColor() { return this->color; }

void PointLightObj::SetColor(Color _newCol) {
	this->color = _newCol;
	this->GetComponentType<MeshRenderer>()->GetMaterial()->SetColor(_newCol);
	this->GetComponentType<PointLight>()->SetColor(_newCol);
}

float PointLightObj::GetIntensity() { return this->intensity; }

void PointLightObj::SetIntensity(float _newIntensity) {
	this->intensity = _newIntensity;
	this->GetComponentType<PointLight>()->SetIntensity(_newIntensity);
}

float PointLightObj::GetRange() { return this->range; }

void PointLightObj::SetRange(float _newRange) {
	this->range = _newRange;
	this->GetComponentType<PointLight>()->SetRange(_newRange);
	this->transform.scale = Vector3(_newRange / rangeScaleMod, _newRange / rangeScaleMod, _newRange / rangeScaleMod);
}
