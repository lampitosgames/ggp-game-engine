#ifndef GGP_DIRECTIONAL_LIGHT_H
#define GGP_DIRECTIONAL_LIGHT_H

#include <SimpleMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
class GameObject;

class DirLight : public BaseComponent<DirLight> {
	DirLightStruct lightData;
public:
	DirLight(GameObject* _gameObject,
			 DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),
			 DirectX::SimpleMath::Vector3 _direction = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f),
			 float _diffuseIntensity = 1.0f,
			 float _ambientIntensity = 0.0f);
	~DirLight();

	DirLightStruct buildLightStruct();

	GameObject* gameObject;
};

#endif //GGP_DIRECTIONAL_LIGHT_H