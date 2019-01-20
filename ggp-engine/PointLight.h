#ifndef GGP_POINT_LIGHT_H
#define GGP_POINT_LIGHT_H

#include <DirectXMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
class GameObject;

class PointLight : public ECS::BaseComponent<PointLight> {
	PointLightStruct lightData;
public:
	PointLight(GameObject* _gameObject);
	PointLight(GameObject* _gameObject, DirectX::XMFLOAT4 _color, float _intensity = 1.0f, float _range = 5.0f);
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

	GameObject* gameObject;
};

#endif //GGP_POINT_LIGHT_H