#ifndef GGP_POINT_LIGHT_H
#define GGP_POINT_LIGHT_H

#include <DirectXMath.h>
#include "Component.h"
#include "LightStructs.h"
class Spatial;

class PointLight : public Component {
	PointLightStruct lightData;
public:
	Spatial* gameObject;
	PointLight(UINT _uniqueID, Spatial* _gameObject);
	PointLight(UINT _uniqueID, Spatial* _gameObject, DirectX::XMFLOAT4 _color, float _intensity = 1.0f, float _constAtten = 1.0f, float _linearAtten = 0.0f, float _expAtten = 1.0f);
	~PointLight();

	//Light color
	DirectX::XMFLOAT4 GetColor();
	void SetColor(DirectX::XMFLOAT4 _color);
	//Light's intensity
	float GetIntensity();
	void SetIntensity(float _diffuseStrength);
	//Attenuation
	float GetConstAtten();
	float GetLinearAtten();
	float GetExpAtten();
	void SetConstAtten(float _constAtten);
	void SetLinearAtten(float _linearAtten);
	void SetExpAtten(float _expAtten);
	PointLightStruct buildLightStruct();
};

#endif //GGP_POINT_LIGHT_H