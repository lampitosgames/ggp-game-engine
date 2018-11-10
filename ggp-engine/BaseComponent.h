#pragma once

#include "IComponent.h"

namespace ECS
{
    using ComponentTypeId = size_t;

    /// <summary>
    /// A base component class for all other component types
    /// to inherit from, that runtime type checking implemented.
    /// </summary>
    template <class T>
    class BaseComponent : public IComponent
    {
        /** Count of component types */
        static ComponentTypeId s_count;

    public:

        BaseComponent() {}

        virtual ~BaseComponent() {}

        static const ComponentTypeId STATIC_COMPONENT_TYPE_ID;

        inline ComponentTypeId GetStaticComponentTypeID() const
        {
            return STATIC_COMPONENT_TYPE_ID;
        }

    };  // BaseComponent

    // This private member only exists to force the compiler to create an instance of Component T,
    // which will set its unique identifier.
    template<class T>
    const ComponentTypeId BaseComponent<T>::STATIC_COMPONENT_TYPE_ID = s_count++;

}   // namespace ECS