#pragma once

#include <string>

namespace ECS {

	using ComponentID = size_t;
	using ComponentTypeId = size_t;
	using EntityID = std::string;

	class GameObject;

	/// <summary>
	/// Abstract component class with basic functionality that every 
	/// component should have.
	/// </summary>
	class IComponent {
		friend class ComponentManager;

	public:

		IComponent() {}

		virtual ~IComponent() {}

		inline const bool operator==(const IComponent& other) const { return id == other.id; }
		inline const bool operator!=(const IComponent& other) const { return id == other.id; }

		/////////////////////////////////////////////////    
		// Accessors 

		inline const ComponentID GetComponentId() const { return this->id; }

		inline const bool IsEnabled() const { return this->isEnabled; }

		inline void SetSenabled(bool aEnabledState) { this->isEnabled = aEnabledState; }

		const EntityID& GetOwner() const { return this->owner; }

	protected:

		/** The unique ID of this component */
		ComponentID id = -1;

		/** If this component is enabled or not */
		bool isEnabled = true;

		/** The owner of this component */
		EntityID owner;

	};  // IComponent

}   // namespace ECS
