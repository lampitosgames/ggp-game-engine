#include "Spatial.h"
#include "RenderManager.h"
#include "LightManager.h"

using namespace DirectX;

Spatial::Spatial(std::string _uniqueID, XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale) : GameObject(_uniqueID) {
	transform = Transform(_position, _rotation, _scale);
}

bool Spatial::HasTransform() {
	return true;
}

void Spatial::SetParent(GameObject* _newParent) {
	//Store pointer to new parent
	parent = _newParent;
	//Store whether or not the parent has a transform. This ensures we don't have to make the check every update, only when parent changes
	parentHasTransform = parent->HasTransform();
	//Set transform parent if parent has transform
	if (parentHasTransform) {
		transform.parent = &((Spatial*)_newParent)->transform;
	}
}

void Spatial::AddChild(GameObject* _newChild) {
	_newChild->SetParent(this);
	if (_newChild->HasTransform()) {
		((Spatial*)_newChild)->transform.parent = &transform;
	}
	children.push_back(_newChild);
	childCount++;
}

void Spatial::AddMeshRenderer() {
	components[CompType::MESH_RENDERER] = renderManager->AddMeshRenderer(this);
}

void Spatial::AddPointLight(XMFLOAT4 _color) {
	components[CompType::POINT_LIGHT] = lightManager->AddPointLight(this, _color);
}
