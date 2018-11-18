#ifndef GGP_SPOT_LIGHT_H
#define GGP_SPOT_LIGHT_H

#include <DirectXMath.h>
#include "Component.h"
#include "LightStructs.h"
#include "GameObject.h"
class GameObject;

class SpotLight : public Component {
	SpotLightStruct lightData;
public:
	GameObject* gameObject;
	SpotLight(UINT _uniqueID, GameObject* _gameObject);
	SpotLight(UINT _uniqueID, GameObject* _gameObject, DirectX::XMFLOAT4 _color, 
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
};

#endif //GGP_SPOT_LIGHT_H