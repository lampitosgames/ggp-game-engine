#include "Spatial.h"
#include "RenderManager.h"
#include "LightManager.h"

using namespace DirectX;

Spatial::Spatial(std::string _uniqueID, XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale) : GameObject(_uniqueID) {
	transform = Transform(_position, _rotation, _scale);
}

void Spatial::AddMeshRenderer() {
	components[CompType::MESH_RENDERER] = renderManager->AddMeshRenderer(this);
}

void Spatial::AddPointLight(XMFLOAT4 _color) {
	components[CompType::POINT_LIGHT] = lightManager->AddPointLight(this, _color);
}
