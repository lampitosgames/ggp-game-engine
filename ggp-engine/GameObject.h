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
#include <map>
#include "GameObjectTypes.h"
#include "ComponentTypes.h"
class MeshManager;

typedef unsigned int UINT;

class GameObject {
protected:
	//Static, engine-wide map of all gameobjects.
	static std::map<std::string, GameObject*> goUIDMap;

	//Unique identifier of this object. Can be anything, must be unique.
	std::string uniqueID;
	//Every game object (and dervived type) will have a unique type so casting can be done
	GOType type;

	//Map of this game object's components
	//Storing the component's unique identifier helps prevent a messy solution to object slicing
	//The resulting function calls to get a component are the same, but components are stored 
	//by their respective providers in type-specific std::maps where slicing isn't an issue.
	std::map<CompType, UINT> components;

	//Pointers to required singletons (used for component management)
	MeshManager* meshManager;
public:
	//Constructors
	GameObject(std::string _uniqueID = "NA");
	//TODO: Implement copy constructor and copy asignment operator
	//GameObject(GameObject const& other);
	//GameObject& operator=(GameObject const& other);
	//Destructor
	~GameObject();

	//Static, global get to access any game object by name
	static GameObject* GetGameObject(std::string _uniqueID);
	
	//Get one of this object's components
	template<typename T>
	T* GetComponent(CompType _type);

	//Gets for member variables
	std::string GetUniqueID();
	GOType GetType();

	//Ensure the string is a unique identifier.  Prevents duplicates
	void GenerateUID(std::string &_outString);
private:
	void Init();
	//void Release();
};

template<typename T>
inline T* GameObject::GetComponent(CompType _type) {
	//This is fugly
	//TODO: Ask about a better way to de-reference components
	//Can I switch based on the class type of T?
	switch (_type) {
		case CompType::MESH_RENDERER:
			//Ensure the mesh renderer component exists
			auto tempMR = components.find(_type);
			if (tempMR != components.end()) {
				//Get the mesh renderer using its stored unique ID. Typecast it since this is a template function
				//This should always work.  If it doesn't, you called the function incorrectly
				return (T*)meshManager->GetMeshRenderer(tempMR->second);
			}
			break;
	}
	return nullptr;
}
#endif //GGP_GAME_OBJECT_H
