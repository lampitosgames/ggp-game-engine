#ifndef GGP_SPOT_LIGHT_H
#define GGP_SPOT_LIGHT_H

#include <DirectXMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
class GameObject;

class SpotLight : public ECS::BaseComponent<SpotLight> {
	SpotLightStruct lightData;
public:
	SpotLight(GameObject* _gameObject);
	SpotLight(GameObject* _gameObject, DirectX::XMFLOAT4 _color,
			  DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			  float _cone = 30.0f,
			  float _range = 50.0f);
	~SpotLight();

	//Light color
	DirectX::XMFLOAT4 GetColor();
	void SetColor(DirectX::XMFLOAT4 _color);
	//Light's direction
	DirectX::XMFLOAT3 GetDirection();
	void SetDirection(DirectX::XMFLOAT3 _direction);
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