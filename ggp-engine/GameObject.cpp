#include "stdafx.h"

#include "GameObject.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "LightManager.h"
#include "ParticleManager.h"
#include "ComponentManager.h"

using namespace std;
using namespace DirectX;

map<std::string, GameObject*> GameObject::goUIDMap = map<std::string, GameObject*>();


GameObject::GameObject(string _uniqueID, XMFLOAT3 _position, XMFLOAT3 _rotation, XMFLOAT3 _scale) {
	Init();
	//Ensure the provided ID is unique
	GenerateUID(_uniqueID);
	//Store unique ID
	uniqueID = _uniqueID;

	transform = Transform(_position, _rotation, _scale);

	componentManager = ECS::ComponentManager::GetInstance();

	//Base constructor, this is a game object
	type = GOType::GAME_OBJECT;
	//Set this object as active
	isActive = true;
	//Store a reference to this game object in the global game object map
	goUIDMap[uniqueID] = this;
	//Initialize child count
	childCount = 0;
	//Set parent to nullptr
	parent = nullptr;
	parentHasTransform = false;
}


GameObject::~GameObject() { Release(); }

void GameObject::Start() {
	//Do all initialization steps (adding components, setting transform, etc.) here
	for (UINT i = 0; i < childCount; i++) {
		children[i]->Start();
	}
}

void GameObject::Update(float _deltaTime) {
	//If this gameObject is inactive, do nothing (and stop the child/parent propogation for this branch)
	if (!isActive) { return; }
	//Update all children
	for (UINT i = 0; i < childCount; i++) {
		if (children[i] == nullptr) { RemoveChild(i); }
		children[i]->Update(_deltaTime);
	}
}

void GameObject::Input(InputEvent _event) {}

void GameObject::SetParent(GameObject* _newParent) {
	//Store pointer to new parent
	parent = _newParent;
	transform.parent = &_newParent->transform;
}

GameObject* GameObject::GetParent() {
	return parent;
}

void GameObject::AddChild(GameObject* _newChild) {
	_newChild->SetParent(this);
	_newChild->transform.parent = &transform;
	children.push_back(_newChild);
	++childCount;
}

void GameObject::RemoveChild(std::string _uniqueID, bool _decrimentChildCount) {
	for (UINT i = 0; i < childCount; i++) {
		if (children[i] == nullptr) { continue; }
		if (children[i]->uniqueID == _uniqueID) {
			if (_decrimentChildCount) {
				//Swap this child with the last child
				children[i] = children[--childCount];
				children[childCount] = nullptr;
				break;
			}
			else {
				children[i] = nullptr;
				break;
			}
		}
	}
}

void GameObject::RemoveChild(UINT _index, bool _decrimentChildCount) {
	if (_index < childCount) {
		if (_decrimentChildCount) {
			children[_index] = children[--childCount];
			children[childCount] = nullptr;
		}
		else {
			children[_index] = nullptr;
		}
	}
}

void GameObject::DeleteChild(std::string _uniqueID) {
	for (UINT i = 0; i < childCount; i++) {
		if (children[i]->uniqueID == _uniqueID) {
			//Swap this child with the last child
			GameObject* tempPointer = children[i];
			children[i] = children[--childCount];
			children[childCount] = nullptr;
			delete tempPointer;
			break;
		}
	}
}

void GameObject::DeleteChild(UINT _index) {
	if (_index < childCount) {
		GameObject* tempPointer = children[_index];
		children[_index] = children[--childCount];
		children[childCount] = nullptr;
		delete tempPointer;
	}
}

bool GameObject::HasChild(std::string _uniqueID) {
	for (UINT i = 0; i < childCount; i++) {
		if (children[i]->uniqueID == _uniqueID) {
			return true;
		}
	}
	return false;
}

GameObject* GameObject::GetChild(std::string _uniqueID) {
	for (UINT i = 0; i < childCount; i++) {
		if (children[i]->uniqueID == _uniqueID) {
			return children[i];
		}
	}
	return nullptr;
}

GameObject* GameObject::GetChild(UINT _index) {
	if (_index < childCount) {
		return children[_index];
	}
	return nullptr;
}

string GameObject::GetUniqueID() { return uniqueID; }

GOType GameObject::GetType() { return type; }

bool GameObject::IsActive() { return isActive; }

void GameObject::Enable() { isActive = true; }

void GameObject::Disable() { isActive = false; }

//Ensures the object's unique identifier is unique to prevent overlaps in the global game object map
void GameObject::GenerateUID(string &_outString) {
	//Static unique index for generated names.
	//Just increments for every object that calls this function to save iterations
	static unsigned int uniqueIndex = 0;
	goUIDMap;
	//Store the starting name of the object
	string startName = _outString;
	//Check to see if the name is already unique.
	GameObject* checkNameObject = GetGameObject<GameObject>(_outString);
	//While there is an object with the generated name
	while (checkNameObject) {
		_outString = startName + "_" + to_string(uniqueIndex);
		uniqueIndex++;
		checkNameObject = GetGameObject<GameObject>(_outString);
	}
	//Once name is unique, return
	return;
}

void GameObject::Init() {
	//Get instances of all singletons
	renderManager = RenderManager::GetInstance();
	inputManager = InputManager::GetInstance();
	resourceManager = ResourceManager::GetInstance();
	lightManager = LightManager::GetInstance();
	particleManager = ParticleManager::GetInstance();
}

void GameObject::Release() {
	//Remove self from the global list of gameObjects
	goUIDMap.erase(uniqueID);
	//Delete components
	componentManager->CleanupComponents(uniqueID);

	//Delete all children
	for (UINT i = 0; i < childCount; i++) {
		if (children[i] != nullptr) {
			delete children[i];
		}
	}

	if (parent != nullptr) {
		parent->RemoveChild(uniqueID, false);
	}
}
