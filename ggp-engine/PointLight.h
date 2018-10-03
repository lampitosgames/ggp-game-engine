#ifndef GGP_POINT_LIGHT_H
#define GGP_POINT_LIGHT_H

#include "Component.h"
#include <DirectXMath.h>
class Spatial;

struct PointLightStruct {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 position;
	float ambientColor;
	float diffuseColor;
	// L = (AmbientColor + diffuseColor)/(constAtten + linearAtten*dist + expAtten*dist*dist)
	float constAtten;
	float linearAtten;
	float expAtten;
};

class PointLight : public Component {
	PointLightStruct lightData;
public:
	Spatial* gameObject;
	PointLight(UINT _uniqueID, Spatial* _gameObject);
	PointLight(UINT _uniqueID, Spatial* _gameObject, DirectX::XMFLOAT4 _color, float _ambientStrength = 0.05f, float _diffuseStrength = 1.0f, float _constAtten = 1.0f, float _linearAtten = 0.0f, float _expAtten = 1.0f);
	~PointLight();

	PointLightStruct buildLightStruct();
};

#endif //GGP_POINT_LIGHT_H