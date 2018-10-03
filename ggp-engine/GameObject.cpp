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
}

GameObject::~GameObject() { Release(); }

GameObject* GameObject::GetGameObject(string _uniqueID) {
	//Find an object in the map with this unique ID
	//We have to use the find() function because accessing a 
	//pair that doesn't exist directly creates an empty one for some reason
	auto foundObject = goUIDMap.find(_uniqueID);
	//If nothing was found, return null pointer
	if (foundObject == goUIDMap.end()) {
		return nullptr;
	}
	//Return found object
	return foundObject->second;
}

void GameObject::Start() {
	//Do all initialization steps (adding components, setting transform, etc.) here
}

void GameObject::Update(float _deltaTime) {
	//If this gameObject is inactive, do nothing (and stop the child/parent propogation for this branch)
	if (!isActive) { return; }
	//TODO: update all children
}

void GameObject::Input(InputEvent _event) {}

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
	GameObject* checkNameObject = GetGameObject(_outString);
	//While there is an object with the generated name
	while (checkNameObject) {
		_outString = startName + "_" + to_string(uniqueIndex);
		uniqueIndex++;
		checkNameObject = GetGameObject(_outString);
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
}
