#include "stdafx.h"
#include "SpotLight.h"
#include "GameObject.h"
using namespace DirectX;

SpotLight::SpotLight(UINT _uniqueID, GameObject * _gameObject)
{
	//call the fully parameterized constructor
	SpotLight(_uniqueID, _gameObject, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

SpotLight::SpotLight(UINT _uniqueID, GameObject * _gameObject, DirectX::XMFLOAT4 _color, DirectX::XMFLOAT3 _direction, float _cone, float _range)
{
	type = CompType::SPOT_LIGHT;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	//store light data
	lightData = { 
		_color,
		_direction,
		_cone,
		gameObject->transform.position,
		_range
	};
}

SpotLight::~SpotLight(){}

DirectX::XMFLOAT4 SpotLight::GetColor()
{
	return lightData.color;
}

void SpotLight::SetColor(DirectX::XMFLOAT4 _color)
{
	lightData.color = _color;
}

DirectX::XMFLOAT3 SpotLight::GetDirection()
{
	return lightData.direction;
}

void SpotLight::SetDirection(DirectX::XMFLOAT3 _direction)
{
	lightData.direction = _direction;
}



float SpotLight::GetCone()
{
	return lightData.cone;
}

void SpotLight::SetCone(float _cone)
{
	lightData.cone = _cone;
}

float SpotLight::GetRange()
{
	return lightData.range;
}

void SpotLight::SetRange(float _newrange)
{
	lightData.range = _newrange;
}

SpotLightStruct SpotLight::buildLightStruct()
{
	lightData.position = gameObject->transform.position;
	return lightData;
}
