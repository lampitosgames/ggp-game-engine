#include "stdafx.h"

#include "SpotLightObj.h"
#include "ResourceManager.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "SpotLight.h"

using namespace std;
using namespace DirectX::SimpleMath;

string SpotLightObj::MakeUIDString(Color _color) {
	string colorID = "rgba(" + to_string(_color.R()) + "," + to_string(_color.G()) + "," + to_string(_color.B()) + "," + to_string(_color.A()) + ")";
	return colorID;
}

Vector3 SpotLightObj::GetEulerFromDir(Vector3 _dir) {
	_dir.Normalize();
	//Make a quaternion out of the dir vector
	Quaternion q = Quaternion::CreateFromAxisAngle(_dir, 30.0f);
	//Convert quaternion back to roll, pitch, yaw (how the transform currently accepts rotations)
	//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	//Roll
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	float rollX = atan2(sinr_cosp, cosr_cosp);
	//Pitch
	float pitchY;
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1.0f) {
		pitchY = copysign(3.14159f / 2.0f, sinp);
	}
	else {
		pitchY = asin(sinp);
	}
	//Yaw
	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	float yawZ = atan2(siny_cosp, cosy_cosp);

	return Vector3(rollX, pitchY, yawZ);
}

SpotLightObj::SpotLightObj(
	ResName _uniqueID, 
	Vector3 _position, 
	Color _color, 
	Vector3 _direction, 
	float _coneAngle, 
	float _range) : GameObject(
		_uniqueID == "NA" ? MakeUIDString(_color) : _uniqueID, 
		_position, 
		_direction, 
		Vector3(1.0f, 1.0f, 1.0f)
	) {
	this->type = GOType::SPOT_LIGHT;
	this->color = _color;
	this->direction = _direction;
	this->coneAngle = _coneAngle;
	this->range = _range;
}

void SpotLightObj::Start() {
	//Create a material that is the same color as the light
	Material* lightColorMat = resourceManager->AddMaterial(MakeUIDString(this->color), res["shdr"]["vert"]["default"], res["shdr"]["pix"]["debug"], this->color);
	//Spotlights are represented by a cone
	Mesh* coneMesh = resourceManager->GetMesh(res["mesh"]["cone"]);
	//Create a mesh renderer
	MeshRenderer* lightMR = this->AddComponent<MeshRenderer>(this);
	lightMR->SetMaterial(lightColorMat);
	lightMR->SetMesh(coneMesh);
	this->AddComponent<SpotLight>(this, this->color, this->direction, this->coneAngle, this->range);
}
