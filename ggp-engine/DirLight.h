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
					 DirectX::XMFLOAT4 _ambientColor = DirectX::XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f), 
					 DirectX::XMFLOAT4 _diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
					 DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	~DirLight();

	DirLightStruct buildLightStruct();
};

#endif //GGP_DIRECTIONAL_LIGHT_H