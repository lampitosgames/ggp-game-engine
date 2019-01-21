#ifndef GGP_COMPONENT_MANAGER_H
#define GGP_COMPONENT_MANAGER_H

#include <vector>
#include <unordered_map>
#include <string>
#include "IComponent.h"

//Component manager that keeps track of what components
//currently exist
class ComponentManager {
	friend class IComponent;
	//Static instance of this component manager
	static ComponentManager* instance;
public:
	//Get a pointer to the instance of the component manager
	static ComponentManager* GetInstance();
	//Destroy the current instance of the component manager and all active instances of components
	static void ReleaseInstance();
	//Delete operators
	ComponentManager(const ComponentManager&) = delete;
	ComponentManager& operator=(ComponentManager&) = delete;

	// Clean up an entity's components
	void CleanupComponents(const EntityID aEntityID);

	//Add a component to the given Entity
	//aEntityID = The entity to put this component on
	//args = Arguments for your component's constructor
	template<class T, class ...ARGS>
	T* AddComponent(EntityID _entityID, ARGS&&... _args) {
		IComponent* newComponent = new T(std::forward<ARGS>(_args)...);
		const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;
		newComponent->owner = _entityID;
		newComponent->id = ComponentCount++;
		this->activeComponents[_entityID][CTID] = newComponent;
		return static_cast<T*>(newComponent);
	}
	//Add a component of a given type without passing constructor args
	template <class T>
	T* GetComponent(const EntityID _entityID) {
		const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;
		return static_cast<T*>(activeComponents[_entityID][CTID]);
	}
	template <class T>
	void RemoveComponent(const EntityID _entityID) {
		const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;
		delete (activeComponents[_entityID][CTID]);
		activeComponents[_entityID][CTID] = nullptr;
	}
private:
	//Private constructor/destructor
	ComponentManager();
	~ComponentManager();

	//Delete all active components
	void CleanupAllComponents();

	//Keep track of all component types
	typedef std::unordered_map<ComponentTypeId, IComponent*> ComponentMap;
	std::unordered_map<EntityID, ComponentMap> activeComponents;

	//Current count of all components
	size_t ComponentCount = 0;

};
#endif //GGP_COMPONENT_MANAGER_H