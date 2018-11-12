#include "SpotLight.h"

SpotLight::SpotLight(UINT _uniqueID, GameObject * _gameObject)
{
}

SpotLight::SpotLight(UINT _uniqueID, GameObject * _gameObject, DirectX::XMFLOAT4 _color, DirectX::XMFLOAT3 _direction, float _cone, float _range)
{
}

SpotLight::~SpotLight()
{
}

DirectX::XMFLOAT4 SpotLight::GetColor()
{
	return DirectX::XMFLOAT4();
}

void SpotLight::SetColor(DirectX::XMFLOAT4 _color)
{
}

DirectX::XMFLOAT3 SpotLight::GetDirection()
{
	return DirectX::XMFLOAT3();
}

void SpotLight::SetDirection(float _direction)
{
}

float SpotLight::GetCone()
{
	return 0.0f;
}

void SpotLight::SetCone(float _cone)
{
}

float SpotLight::GetRange()
{
	return 0.0f;
}

void SpotLight::SetRange(float _newrange)
{
}

PointLightStruct SpotLight::buildLightStruct()
{
	return PointLightStruct();
}
