#ifndef GGP_BASE_COMPONENT
#define GGP_BASE_COMPONENT

#include "IComponent.h"

namespace ECS {
	namespace Util {
		using TypeID = uint32_t;
		template<class T>
		class FamilyTypeID {
			static TypeID s_count;
		public:
			template<class U>
			static const TypeID Get() {
				static const TypeID STATIC_TYPE_ID{ s_count++ };
				return STATIC_TYPE_ID;
			}
			static const TypeID Get() {
				return s_count;
			}
		};
	}

	using ComponentTypeId = size_t;
	//A base component class for all other component types
	//to inherit from, that runtime type checking implemented.
	template <class T>
	class BaseComponent : public IComponent {
	public:
		//Count of component types
		static const ComponentTypeId STATIC_COMPONENT_TYPE_ID;
		BaseComponent() {}
		virtual ~BaseComponent() {}

		ComponentTypeId GetStaticComponentTypeID() const { return STATIC_COMPONENT_TYPE_ID;	}
	};

	// This private member only exists to force the compiler to create an instance of Component T,
	// which will set its unique identifier.
	template<class T>
	const ComponentTypeId ECS::BaseComponent<T>::STATIC_COMPONENT_TYPE_ID = ECS::Util::FamilyTypeID<IComponent>::Get<T>();
}
#endif //GGP_BASE_COMPONENT