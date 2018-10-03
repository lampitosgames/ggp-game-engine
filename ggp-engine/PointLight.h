#ifndef GGP_POINT_LIGHT_H
#define GGP_POINT_LIGHT_H

#include <DirectXMath.h>
#include "Component.h"
#include "LightStructs.h"
class Spatial;

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