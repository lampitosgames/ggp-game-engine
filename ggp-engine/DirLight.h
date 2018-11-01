#ifndef GGP_DIRECTIONAL_LIGHT_H
#define GGP_DIRECTIONAL_LIGHT_H

#include <DirectXMath.h>
#include "Component.h"
#include "LightStructs.h"
class GameObject;

class DirLight : public Component {
	DirLightStruct lightData;
public:
	GameObject* gameObject;

	DirLight(UINT _uniqueID, 
			 GameObject* _gameObject, 
			 DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			 DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
			 float _diffuseIntensity = 1.0f,
			 float _ambientIntensity = 0.0f);
	~DirLight();

	DirLightStruct buildLightStruct();
};

#endif //GGP_DIRECTIONAL_LIGHT_H