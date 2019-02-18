#ifndef GGP_LIGHT_H
#define GGP_LIGHT_H

#include <SimpleMath.h>

struct DirLightStruct {
	DirectX::SimpleMath::Color color;
	DirectX::SimpleMath::Vector3 direction;
	float diffuseIntensity;
	float ambientIntensity;
	DirectX::SimpleMath::Vector3 empty;
};

struct PointLightStruct {
	DirectX::SimpleMath::Color color;
	DirectX::SimpleMath::Vector3 position;
	float intensity;
	float range;
	DirectX::SimpleMath::Vector3 empty;
};

struct SpotLightStruct {
	DirectX::SimpleMath::Color color;
	DirectX::SimpleMath::Vector3 direction;
	float cone;
	DirectX::SimpleMath::Vector3 position;
	float range;
};
#endif //GGP_LIGHT_H