#ifndef GGP_POINT_LIGHT_H
#define GGP_POINT_LIGHT_H

#include <DirectXMath.h>
#include "Component.h"
#include "LightStructs.h"
class Spatial;

class SpotLight : public Component {
	SpotLightStruct lightData;
public:
	Spatial* gameObject;
	SpotLight(UINT _uniqueID, Spatial* _gameObject);
	SpotLight(UINT _uniqueID, Spatial* _gameObject, DirectX::XMFLOAT4 _color,
		DirectX::XMFLOAT3 _direction =  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
		float _cone = 15.0f, 
		float _range = 10.0f);
	~SpotLight();

	//Light color
	DirectX::XMFLOAT4 GetColor();
	void SetColor(DirectX::XMFLOAT4 _color);
	DirectX::XMFLOAT3 GetDirection();
	void SetDirection(DirectX::XMFLOAT3 _direction);
	void SetCone(float _cone);
	float GetRange();
	void SetRange(float _range);
	
	SpotLightStruct buildLightStruct();
};

#endif //GGP_POINT_LIGHT_H#pragma once
