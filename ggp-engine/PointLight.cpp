#include "PointLight.h"
#include "Spatial.h"
using namespace DirectX;



PointLight::PointLight(UINT _uniqueID, Spatial* _gameObject) {
	//Call the fully parameterized constructor
	PointLight(_uniqueID, _gameObject, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

PointLight::PointLight(UINT _uniqueID, Spatial* _gameObject, XMFLOAT4 _color, float _ambientStrength, float _diffuseStrength, float _constAtten, float _linearAtten, float _expAtten) {
	type = CompType::POINT_LIGHT;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	//Store light data
	lightData = {
		_color,
		gameObject->transform.position,
		_ambientStrength,
		_diffuseStrength,
		_constAtten,
		_linearAtten,
		_expAtten
	};
}

PointLight::~PointLight() {}

PointLightStruct PointLight::buildLightStruct() {
	lightData.position = gameObject->transform.position;
	return lightData;
}
