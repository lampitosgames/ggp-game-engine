#ifndef GGP_GAME_OBJECT_H
#define GGP_GAME_OBJECT_H

//Temp Note to people reading this:
/*
	So this is a bit of a mess right now because I'm storing lots of things
	in temporary variables I'll refactor later.  I have the whole high-level
	engine design on paper but it obviously isn't able to be implemented in
	a week so for now I'm just going to have to deal with some spaghetti
*/

#include <DirectXMath.h>
#include <map>
#include <vector>
#include "GameObjectTypes.h"
#include "InputEvent.h"
#include "Transform.h"
#include "ComponentManager.h"
class InputManager;
class LightManager;
class ResourceManager;
class RenderManager;
class ParticleManager;

typedef unsigned int UINT;

class GameObject {
public:
    //Static, engine-wide map of all gameobjects.
    static std::map<std::string, GameObject*> goUIDMap;
protected:



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

	//Pointers to required singletons (used for component management)
	RenderManager* renderManager;
	InputManager* inputManager;
	ResourceManager* resourceManager;
	LightManager* lightManager;
	ParticleManager* particleManager;

	ECS::ComponentManager* componentManager = nullptr;

public:

	GameObject(std::string _uniqueID = "NA",
			   DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			   DirectX::XMFLOAT3 _rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			   DirectX::XMFLOAT3 _scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

		   //Destructor
	~GameObject();

	Transform transform;

	//Static, global get to access any game object by name
	//static GameObject* GetGameObject(std::string _uniqueID);
	//Override of GetGameObject that lets you pass in a type
	template<typename T>
	static T* GetGameObject(std::string _uniqueID) {
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
	void RemoveChild(std::string _uniqueID, bool _decrimentChildCount = true);
	void RemoveChild(UINT _index, bool _decrimentChildCount = true);
	//Removes and deletes a child object
	void DeleteChild(std::string _uniqueID);
	void DeleteChild(UINT _index);
	//Check if a child exists
	bool HasChild(std::string _uniqueID);
	//Get the child
	GameObject* GetChild(std::string _uniqueID);
	GameObject* GetChild(UINT _index);

	//Get one of this object's components
	template<typename T>
	T* GetComponentType() {
		return this->componentManager->GetComponent<T>(this->uniqueID);
	}

	template<class T, class ...P>
	T* AddComponent(P&&... param) {
		return
			this->componentManager->AddComponent<T>(
				this->uniqueID,
				std::forward<P>(param)...
				);
	}

	template<typename T>
	void RemoveComponent() {
		this->componentManager->RemoveComponent<T>(this->uniqueID);
	}

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

#endif //GGP_GAME_OBJECT_H
