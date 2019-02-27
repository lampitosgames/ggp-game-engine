#ifndef GGP_SPOT_LIGHT_OBJ_H
#define GGP_SPOT_LIGHT_OBJ_H

#include "GameObject.h"

class SpotLightObj : public GameObject {
protected:
	DirectX::SimpleMath::Color color;
	DirectX::SimpleMath::Vector3 direction;
	float coneAngle;
	float range;

	std::string MakeUIDString(DirectX::SimpleMath::Color _color);
	DirectX::SimpleMath::Vector3 GetEulerFromDir(DirectX::SimpleMath::Vector3 _dir);
public:
	SpotLightObj(
		ResName _uniqueID = "NA",
		DirectX::SimpleMath::Vector3 _position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),
		DirectX::SimpleMath::Vector3 _direction = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f),
		float _coneAngle = 30.0f,
		float _range = 10.0f
	);
	void Start();

	DirectX::SimpleMath::Color GetColor();
	void SetColor(DirectX::SimpleMath::Color _newCol);

};

#endif //GGP_SPOT_LIGHT_OBJ_H