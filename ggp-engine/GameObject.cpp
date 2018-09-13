#include "GameObject.h"
#include "MeshManager.h"
using namespace std;

map<std::string, GameObject*> GameObject::goUIDMap = map<std::string, GameObject*>();

GameObject::GameObject(string _uniqueID) {
	Init();
	//Ensure the provided ID is unique
	GenerateUID(uniqueID);
	//Store unique ID
	uniqueID = _uniqueID;
	//Base constructor, this is a game object
	type = GOType::GAME_OBJECT;
	//Store a reference to this game object in the global game object map
	goUIDMap[uniqueID] = this;
}

GameObject::~GameObject() {
}

GameObject* GameObject::GetGameObject(string _uniqueID) {
	auto foundObject = goUIDMap.find(_uniqueID);
	if (foundObject == goUIDMap.end()) {
		return nullptr;
	}
	return foundObject->second;
}

string GameObject::GetUniqueID() {
	return uniqueID;
}

GOType GameObject::GetType() {
	return type;
}

//Ensures the object's unique identifier is unique to prevent overlaps in the global game object map
void GameObject::GenerateUID(string &_outString) {
	//Static unique index for generated names.
	//Just increments for every object that calls this function to save iterations
	static unsigned int uniqueIndex = 0;
	//Store the starting name of the object
	string startName = _outString;
	//Check to see if the name is already unique.
	GameObject* checkNameObject = GetGameObject(_outString);
	//While there is an object with the generated name
	while (checkNameObject != nullptr) {
		_outString = startName + "_" + to_string(uniqueIndex);
		uniqueIndex++;
		checkNameObject = GetGameObject(_outString);
	}
	//Once name is unique, return
	return;
}

void GameObject::Init() {
	meshManager = MeshManager::GetInstance();
}
