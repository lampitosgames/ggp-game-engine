#ifndef GGP_GAME_OBJECT_H
#define GGP_GAME_OBJECT_H

#include <map>
#include <vector>
#include <SimpleMath.h>
#include "json.hpp"
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
    static std::map<ResName, GameObject*> goUIDMap;
protected:
	//Unique identifier of this object. Can be anything, must be unique.
	ResName uniqueID;
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

	//JSON map of resource filepaths
	nlohmann::json res;

	//Pointers to required singletons (used for component management)
	RenderManager* renderManager;
	InputManager* inputManager;
	ResourceManager* resourceManager;
	LightManager* lightManager;
	ParticleManager* particleManager;
	ComponentManager* componentManager;

public:

	GameObject(ResName _uniqueID = "NA",
			   DirectX::SimpleMath::Vector3 _position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
			   DirectX::SimpleMath::Vector3 _rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
			   DirectX::SimpleMath::Vector3 _scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));

		   //Destructor
	~GameObject();

	Transform transform;

	//Static, global get to access any game object by name
	//static GameObject* GetGameObject(std::string _uniqueID);
	//Override of GetGameObject that lets you pass in a type
	template<typename T>
	static T* GetGameObject(ResName _uniqueID) {
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
	void RemoveChild(ResName _uniqueID, bool _decrimentChildCount = true);
	void RemoveChild(UINT _index, bool _decrimentChildCount = true);
	//Removes and deletes a child object
	void DeleteChild(ResName _uniqueID);
	void DeleteChild(UINT _index);
	//Check if a child exists
	bool HasChild(ResName _uniqueID);
	//Get the child
	GameObject* GetChild(ResName _uniqueID);
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
