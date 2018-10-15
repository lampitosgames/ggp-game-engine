#include "GameObject.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "LightManager.h"
using namespace std;

map<std::string, GameObject*> GameObject::goUIDMap = map<std::string, GameObject*>();

GameObject::GameObject(string _uniqueID) {
	Init();
	//Ensure the provided ID is unique
	GenerateUID(_uniqueID);
	//Store unique ID
	uniqueID = _uniqueID;
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
		children[i]->Update(_deltaTime);
	}
}

void GameObject::Input(InputEvent _event) {}

void GameObject::SetParent(GameObject* _newParent) { 
	//Store pointer to new parent
	parent = _newParent;
	//Store whether or not the parent has a transform. This ensures we don't have to make the check every update, only when parent changes
	parentHasTransform = parent->HasTransform();
}

GameObject* GameObject::GetParent() {
	return parent;
}

void GameObject::AddChild(GameObject* _newChild) {
	children.push_back(_newChild);
	childCount++;
}

void GameObject::RemoveChild(std::string _uniqueID) {
	for (UINT i = 0; i < childCount; i++) {
		if (children[i]->uniqueID == _uniqueID) {
			//Swap this child with the last child
			children[i] = children[--childCount];
			children[childCount] = nullptr;
			break;
		}
	}
}

void GameObject::RemoveChild(UINT _index) {
	if (_index < childCount) {
		children[_index] = children[--childCount];
		children[childCount] = nullptr;
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

bool GameObject::HasTransform() {
	return false;
}

void GameObject::AddInputListener() {
	components[CompType::INPUT_LISTENER] = inputManager->AddInputListener(this);
}

void GameObject::AddDirLight(DirectX::XMFLOAT4 _ambientColor, DirectX::XMFLOAT4 _diffuseColor, DirectX::XMFLOAT3 _direction) {
	components[CompType::DIRECTIONAL_LIGHT] = lightManager->AddDirLight(this, _ambientColor, _diffuseColor, _direction);
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
}

void GameObject::Release() {
	//Remove self from the global list of gameObjects
	goUIDMap.erase(uniqueID);
	//Delete all children
	for (UINT i = 0; i < childCount; i++) {
		delete children[i];
	}
}
