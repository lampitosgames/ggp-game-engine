#pragma once

#include "IComponent.h"
#include "FamilyTypeID.h"

namespace ECS {
	using ComponentTypeId = size_t;

	/// <summary>
	/// A base component class for all other component types
	/// to inherit from, that runtime type checking implemented.
	/// </summary>
	template <class T>
	class BaseComponent : public IComponent {

	public:

		/** Count of component types */
		static const ComponentTypeId STATIC_COMPONENT_TYPE_ID;

		BaseComponent() {}

		virtual ~BaseComponent() {}


		ComponentTypeId GetStaticComponentTypeID() const {
			return STATIC_COMPONENT_TYPE_ID;
		}

	};  // BaseComponent

	// This private member only exists to force the compiler to create an instance of Component T,
	// which will set its unique identifier.
	template<class T>
	const ComponentTypeId ECS::BaseComponent<T>::STATIC_COMPONENT_TYPE_ID = ECS::Util::FamilyTypeID<IComponent>::Get<T>();


}   // namespace ECS