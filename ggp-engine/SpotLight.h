#ifndef GGP_SPOT_LIGHT_H
#define GGP_SPOT_LIGHT_H

#include <SimpleMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
class GameObject;

class SpotLight : public BaseComponent<SpotLight> {
	SpotLightStruct lightData;
public:
	SpotLight(GameObject* _gameObject);
	SpotLight(GameObject* _gameObject, DirectX::SimpleMath::Color _color,
		DirectX::SimpleMath::Vector3 _direction = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f),
		float _cone = 30.0f,
		float _range = 50.0f);
	~SpotLight();

	//Light color
	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _color);
	//Light's direction
	DirectX::SimpleMath::Vector3 GetDirection();
	void SetDirection(DirectX::SimpleMath::Vector3 _direction);
	//cone <-- angle
	float GetCone();
	void SetCone(float _cone);
	//Light's range
	float GetRange();
	void SetRange(float _newrange);
	SpotLightStruct buildLightStruct();

	GameObject* gameObject;
};

#endif //GGP_SPOT_LIGHT_H