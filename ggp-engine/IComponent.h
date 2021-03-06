#ifndef GGP_ICOMPONENT_H
#define GGP_ICOMPONENT_H

#include <string>
using ComponentID = size_t;
using ComponentTypeId = size_t;
using EntityID = std::string;
class GameObject;

//Abstract component class with basic functionality that every 
//component should have.
class IComponent {
	friend class ComponentManager;

public:
	IComponent() {}
	virtual ~IComponent() {}

	inline const bool operator==(const IComponent& other) const { return id == other.id; }
	inline const bool operator!=(const IComponent& other) const { return id == other.id; }

	inline const ComponentID GetComponentId() const { return this->id; }
	inline const bool IsEnabled() const { return this->isEnabled; }
	inline void SetSenabled(bool _enabledState) { this->isEnabled = _enabledState; }
	const EntityID& GetOwner() const { return this->owner; }
protected:
	//The unique ID of this component
	ComponentID id = -1;

	//If this component is enabled or not
	bool isEnabled = true;

	//The owner of this component
	EntityID owner;
};
#endif //GGP_ICOMPONENT_H
