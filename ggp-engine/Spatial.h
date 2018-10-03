#ifndef GGP_SPATIAL_H
#define GGP_SPATIAL_H

#include <DirectXMath.h>
#include <string>
#include "GameObject.h"
#include "Transform.h"

class Spatial : public GameObject {
public:
	Transform transform;

	Spatial(std::string _uniqueID = "NA", 
			DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3 _rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT3 _scale    = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	void AddMeshRenderer();
	void AddPointLight(DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//void AddDirectionalLight();
};

#endif //GGP_SPATIAL_H