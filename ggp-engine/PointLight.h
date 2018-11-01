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
	PointLight(UINT _uniqueID, Spatial* _gameObject, DirectX::XMFLOAT4 _color, float _intensity = 1.0f, float _range = 5.0f);
	~PointLight();

	//Light color
	DirectX::XMFLOAT4 GetColor();
	void SetColor(DirectX::XMFLOAT4 _color);
	//Light's intensity
	float GetIntensity();
	void SetIntensity(float _diffuseStrength);
	//Attenuation
	float GetRange();
	void SetRange(float _newRange);
	PointLightStruct buildLightStruct();
};

#endif //GGP_POINT_LIGHT_H