#ifndef GGP_POINT_LIGHT_H
#define GGP_POINT_LIGHT_H

#include <SimpleMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
class GameObject;

class PointLight : public BaseComponent<PointLight> {
	PointLightStruct lightData;
public:
	PointLight(GameObject* _gameObject);
	PointLight(GameObject* _gameObject, DirectX::SimpleMath::Color _color, float _intensity = 1.0f, float _range = 5.0f);
	~PointLight();

	//Light color
	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _color);
	//Light's intensity
	float GetIntensity();
	void SetIntensity(float _diffuseStrength);
	//Attenuation
	float GetRange();
	void SetRange(float _newRange);

	void GetViewMatrices(DirectX::SimpleMath::Matrix* _viewList);
	DirectX::SimpleMath::Matrix GetProjMatrix();

	PointLightStruct buildLightStruct();

	GameObject* gameObject;
};

#endif //GGP_POINT_LIGHT_H