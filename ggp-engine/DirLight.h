#ifndef GGP_DIRECTIONAL_LIGHT_H
#define GGP_DIRECTIONAL_LIGHT_H

#include <DirectXMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
class GameObject;

class DirLight : public ECS::BaseComponent<DirLight> {
	DirLightStruct lightData;
public:
	DirLight(GameObject* _gameObject,
			 DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			 DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			 float _diffuseIntensity = 1.0f,
			 float _ambientIntensity = 0.0f);
	~DirLight();

	DirLightStruct buildLightStruct();

	GameObject* gameObject;
};

#endif //GGP_DIRECTIONAL_LIGHT_H