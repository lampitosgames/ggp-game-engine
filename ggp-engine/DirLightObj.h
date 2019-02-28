#ifndef GGP_DIR_LIGHT_OBJ_H
#define GGP_DIR_LIGHT_OBJ_H

#include "GameObject.h"

class DirLightObj : public GameObject {
protected:
	static float rangeScaleMod;
	DirectX::SimpleMath::Color color;
	DirectX::SimpleMath::Vector3 direction;
	float diffuseIntensity;
	float ambientIntensity;
public:
	DirLightObj(
		ResName _uniqueID = "NA",
		DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::SimpleMath::Vector3 _direction = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f),
		float _diffuseIntensity = 1.0f,
		float _ambientIntensity = 0.0f
	);
	void Start();

	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _newCol);
	DirectX::SimpleMath::Vector3 GetDirection();
	void SetDirection(DirectX::SimpleMath::Vector3 _newDir);
	float GetDiffuse();
	void SetDiffuse(float _newDI);
	float GetAmbient();
	void SetAmbient(float _newAI);
};

#endif //GGP_DIR_LIGHT_OBJ_H