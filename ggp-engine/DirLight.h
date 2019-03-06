#ifndef GGP_DIRECTIONAL_LIGHT_H
#define GGP_DIRECTIONAL_LIGHT_H

#include <SimpleMath.h>
#include "BaseComponent.h"
#include "LightStructs.h"
#include "SimpleMath.h"
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

	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _newColor);
	DirectX::SimpleMath::Vector3 GetDirection();
	void SetDirection(DirectX::SimpleMath::Vector3 _newDir);
	float GetDiffuse();
	void SetDiffuse(float _newDI);
	float GetAmbient();
	void SetAmbient(float _newAI);

	DirectX::SimpleMath::Matrix GetViewMatrix();
	DirectX::SimpleMath::Matrix GetProjMatrix();

	DirLightStruct buildLightStruct();

	GameObject* gameObject;
};

#endif //GGP_DIRECTIONAL_LIGHT_H