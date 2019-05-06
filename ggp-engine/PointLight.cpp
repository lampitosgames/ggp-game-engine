#include "stdafx.h"

#include "PointLight.h"
#include "GameObject.h"
#include "LightManager.h"
using namespace DirectX::SimpleMath;

PointLight::PointLight(GameObject* _gameObject) {
	//Call the fully parameterized constructor
	PointLight(_gameObject, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

PointLight::PointLight(GameObject* _gameObject, Color _color, float _intensity, float _range) {
	gameObject = _gameObject;
	owner = gameObject->GetUniqueID();
	//Store light data
	lightData = {
		_color,
		gameObject->transform.position,
		_intensity,
		_range
	};

	LightManager::GetInstance()->AddPointLight(this);
}

PointLight::~PointLight() {
	LightManager::GetInstance()->RemovePointLight(this);
}

Color PointLight::GetColor() {
	return lightData.color;
}

void PointLight::SetColor(Color _color) { lightData.color = _color; }

float PointLight::GetIntensity() { return lightData.intensity; }

void PointLight::SetIntensity(float _newIntensity) { lightData.intensity = _newIntensity; }

float PointLight::GetRange() { return lightData.range; }

void PointLight::SetRange(float _newRange) { lightData.range = _newRange; }

void PointLight::GetViewMatrices(Matrix* _viewList) {
	Vector3 thisPos = gameObject->transform.position;
	//Hard-coded vals from https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
	_viewList[0] = Matrix::CreateLookAt(thisPos, thisPos + Vector3(+1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)).Transpose();
	_viewList[1] = Matrix::CreateLookAt(thisPos, thisPos + Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)).Transpose();
	_viewList[2] = Matrix::CreateLookAt(thisPos, thisPos + Vector3(0.0f,+1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)).Transpose();
	_viewList[3] = Matrix::CreateLookAt(thisPos, thisPos + Vector3(0.0f,-1.0f, 0.0f), Vector3(0.0f, 0.0f,-1.0f)).Transpose();
	_viewList[4] = Matrix::CreateLookAt(thisPos, thisPos + Vector3(0.0f, 0.0f,+1.0f), Vector3(0.0f, -1.0f, 0.0f)).Transpose();
	_viewList[5] = Matrix::CreateLookAt(thisPos, thisPos + Vector3(0.0f, 0.0f,-1.0f), Vector3(0.0f, -1.0f, 0.0f)).Transpose();
}

Matrix PointLight::GetProjMatrix() {
	return Matrix::CreatePerspectiveFieldOfView(90.0f, 1.0f, 0.1f, 200.0f).Transpose();
}

PointLightStruct PointLight::buildLightStruct() {
	lightData.position = gameObject->transform.position;
	return lightData;
}
