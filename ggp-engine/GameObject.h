#ifndef GGP_GAME_OBJECT_H
#define GGP_GAME_OBJECT_H

//Temp Note to people reading this:
/*
	So this is a bit of a mess right now because I'm storing lots of things
	in temporary variables I'll refactor later.  I have the whole high-level
	engine design on paper but it obviously isn't able to be implemented in 
	a week so for now I'm just going to have to deal with some spaghetti
*/

#include <string>
#include <DirectXMath.h>
#include <map>
#include <vector>
#include "GameObjectTypes.h"
#include "ComponentTypes.h"
#include "InputEvent.h"
#include "InputManager.h"
#include "LightManager.h"
class ResourceManager;
class RenderManager;

typedef unsigned int UINT;

class GameObject {
protected:
	//Static, engine-wide map of all gameobjects.
	static std::map<std::string, GameObject*> goUIDMap;

	//Unique identifier of this object. Can be anything, must be unique.
	std::string uniqueID;
	//Every game object (and dervived type) will have a unique type so casting can be done
	GOType type;
	//Is the game object active or not? Defaults to true
	bool isActive;

	//Parent pointer
	GameObject* parent;
	bool parentHasTransform;
	
	//Array of children pointers
	std::vector<GameObject*> children;
	UINT childCount;

	//Map of this game object's components
	//Storing the component's unique identifier helps prevent a messy solution to object slicing
	//The resulting function calls to get a component are the same, but components are stored 
	//by their respective providers in type-specific std::maps where slicing isn't an issue.
	std::map<CompType, UINT> components;

	//Pointers to required singletons (used for component management)
	RenderManager* renderManager;
	InputManager* inputManager;
	ResourceManager* resourceManager;
	LightManager* lightManager;
public:
	//Constructors
	GameObject(std::string _uniqueID = "NA");

	//Destructor
	~GameObject();

	//Static, global get to access any game object by name
	//static GameObject* GetGameObject(std::string _uniqueID);
	//Override of GetGameObject that lets you pass in a type
	template<typename T>
	static T* GetGameObject(std::string _uniqueID);

	//Start method
	virtual void Start();

	//Update method
	virtual void Update(float _deltaTime);

	//Input event handler (Will only ever be called by the engine if the game object has an InputListener component)
	virtual void Input(InputEvent _event);

	//Get/Set parent for this object
	virtual void SetParent(GameObject* _newParent);
	GameObject* GetParent();

	//Children methods for this object
	//Adds a child
	virtual void AddChild(GameObject* _newChild);
	//Removes a child without deleting the object
	void RemoveChild(std::string _uniqueID);
	void RemoveChild(UINT _index);
	//Removes and deletes a child object
	void DeleteChild(std::string _uniqueID);
	void DeleteChild(UINT _index);
	//Check if a child exists
	bool HasChild(std::string _uniqueID);
	//Get the child
	GameObject* GetChild(std::string _uniqueID);
	GameObject* GetChild(UINT _index);

	//Does this object have a transform? Always returns false on the base class, but it needs to exist so parent/child transform relationships aren't broken
	virtual bool HasTransform();

	//Get one of this object's components
	template<typename T>
	T* GetComponent(CompType _type);

	//Functions to add different types of components
	void AddInputListener();
	void AddDirLight(DirectX::XMFLOAT4 _ambientColor = DirectX::XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f), DirectX::XMFLOAT4 _diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	//Gets for member variables
	std::string GetUniqueID();
	GOType GetType();

	//GameObject active state:
	bool IsActive();
	void Enable();
	void Disable();

	//Ensure the string is a unique identifier.  Prevents duplicates
	void GenerateUID(std::string &_outString);
private:
	void Init();
	void Release();
};

template<typename T>
inline T* GameObject::GetGameObject(std::string _uniqueID) {
	//Find an object in the map with this unique ID
	//We have to use the find() function because accessing a 
	//pair that doesn't exist directly creates an empty one for some reason
	auto foundObject = goUIDMap.find(_uniqueID);
	//If nothing was found, return null pointer
	if (foundObject == goUIDMap.end()) {
		return nullptr;
	}
	//Return found object
	return (T*)foundObject->second;
}

template<typename T>
inline T* GameObject::GetComponent(CompType _type) {
	//This is fugly
	//TODO: Ask about a better way to de-reference components
	//Can I switch based on the class type of T?
	switch (_type) {
		case CompType::MESH_RENDERER: {
			//Ensure the mesh renderer component exists
			auto tempMR = components.find(_type);
			if (tempMR != components.end()) {
				//Get the mesh renderer using its stored unique ID. Typecast it since this is a template function
				//This should always work.  If it doesn't, you called the function incorrectly
				return (T*)renderManager->GetMeshRenderer(tempMR->second);
			}
		}
		case CompType::INPUT_LISTENER: {
			//Ensure the input listener component exists
			auto tempIL = components.find(_type);
			if (tempIL != components.end()) {
				//Similar to above, get IL from singleton and typecast based on T
				return (T*)inputManager->GetInputListener(tempIL->second);
			}
		}
		case CompType::DIRECTIONAL_LIGHT: {
			//Ensure the component exists
			auto tempDL = components.find(_type);
			if (tempDL != components.end()) {
				//typecast
				return (T*)lightManager->GetDirLight(tempDL->second);
			}
		}
		case CompType::POINT_LIGHT: {
			//Ensure the component exists
			auto tempPL = components.find(_type);
			if (tempPL != components.end()) {
				//typecast
				return (T*)lightManager->GetPointLight(tempPL->second);
			}
		}
	}
	return nullptr;
}
#endif //GGP_GAME_OBJECT_H
