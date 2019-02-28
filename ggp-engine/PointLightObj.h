#ifndef GGP_POINT_LIGHT_OBJ_H
#define GGP_POINT_LIGHT_OBJ_H

#include "GameObject.h"

class PointLightObj : public GameObject {
protected:
	static float rangeScaleMod;
	DirectX::SimpleMath::Color color;
	float intensity;
	float range;
public:
	PointLightObj(
		ResName _uniqueID = "NA",
		DirectX::SimpleMath::Vector3 _position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),
		float _intensity = 1.0f,
		float _range = 5.0f
	);
	void Start();

	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _newCol);
	float GetIntensity();
	void SetIntensity(float _newIntensity);
	float GetRange();
	void SetRange(float _newRange);
};

#endif //GGP_POINT_LIGHT_OBJ_H