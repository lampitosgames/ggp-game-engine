#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "IComponent.h"


namespace ECS
{
    using ComponentTypeId = size_t;
    using EntityID = std::string;

    /// <summary>
    /// Component manager that keeps track of what components
    /// currently exist
    /// </summary>
    class ComponentManager
    {
    public:

        /// <summary>
        /// Get a pointer to the instance of the 
        /// Component manager
        /// </summary>
        /// <returns>Pointer to Component manager</returns>
        static ComponentManager* GetInstance();

        /// <summary>
        /// Destroy the current instance of the component manager
        /// and all active instances of components
        /// </summary>
        static void ReleaseInstance();

        ////////////////////////////////////////////
        // Operators 

        ComponentManager( const ComponentManager& ) = delete;
        ComponentManager& operator=( ComponentManager& ) = delete;


        
        /// <summary>
        /// Add a component to the given Entity
        /// </summary>
        /// <param name="aEntityID">The entity to put this component on</param>
        /// <param name="...args">Arguments for your component's constructor</param>
        /// <returns>Pointer to the newly created component</returns>
        template<class T, class ...ARGS>
        T* AddComponent( const EntityID aEntityID, ARGS&&... args )
        {
            IComponent* newComponent = new T( std::forward<ARGS>( args )... );
            
            const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;

            newComponent->owner = aOwner;
            newComponent->id = ComponentCount;

            ++ComponentCount;
            activeComponents[ EntityID ].push_back( newComponent );

            return static_cast< T* >( newComponent );
        }
        
        template <class T>
        T* GetComponent( const EntityID aEntityID )
        {
            const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;


            return nullptr;
        }

        template <class T>
        void RemoveComponent( const EntityID aEntityID )
        {
            const ComponentTypeId CTID = T::STATIC_COMPONENT_TYPE_ID;

        }
        
    private:

        ComponentManager();
        ~ComponentManager();

        static ComponentManager* instance;

        /// <summary>
        /// Delete all active components
        /// </summary>
        void CleanupAllComponents();

        void MapComponent( size_t aEntityID, ComponentID aComponentID, ComponentTypeId aComponentType );

        // This has pretty bad spatial locality but I don't have time
        // to implement a custom allocator
        typedef std::vector<std::vector<IComponent*>> ComponentMap;

        /** Keep track of all component types */
        std::unordered_map<EntityID, ComponentMap> activeComponents;

        /** Current count of all components */
        size_t ComponentCount = 0;

    };  // ComponentManager

}   // namespace ECS