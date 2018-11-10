#pragma once

#include <vector>

#include "IComponent.h"
#include "GameObject.h"

namespace ECS
{
    /// <summary>
    /// Component manager that keeps track of what components
    /// currently exist
    /// </summary>
    class ComponentManager
    {
    public:

        ComponentManager();
        ~ComponentManager();


        ComponentManager( const ComponentManager& ) = delete;
        ComponentManager& operator=( ComponentManager& ) = delete;



        template<class T, class ...ARGS>
        T* AddComponent( const GameObject* aOwner, ARGS&&... args )
        {
            IComponent* newComponent = new T( std::forward<ARGS>( args )... );
            
            const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;

            newComponent->owner = aOwner;
            newComponent->id = ComponentCount;

            ++ComponentCount;            

            return static_cast< T* >( newComponent );
        }

        template <class T>
        T* GetComponent( const GameObject * aOwner )
        {
            const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;


            return nullptr;
        }

    private:

        /// <summary>
        /// Delete all active components
        /// </summary>
        void CleanupAllComponents();

        void MapComponent( size_t aEntityID, ComponentID aComponentID, size_t aComponentType );

        /** Keep track of all component types */
        std::vector<std::vector<IComponent*>> activeComponents;       

        /** Current count of all components */
        size_t ComponentCount = 0;

    };  // ComponentManager

}   // namespace ECS