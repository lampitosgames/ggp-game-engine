#include "stdafx.h"
#include "ComponentManager.h"

using namespace ECS;

ComponentManager::ComponentManager()
{
}


ComponentManager::~ComponentManager()
{
    CleanupAllComponents();
}

void ComponentManager::CleanupAllComponents()
{
    auto map_itr = activeComponents.begin();

    for ( ; map_itr != activeComponents.end(); ++map_itr )
    {
        auto vec_itr = map_itr->begin();
        for ( ; vec_itr != map_itr->end(); ++vec_itr )
        {
            if ( *( vec_itr ) != nullptr )
            {
                delete *( vec_itr );
            }            
        }
    }
    ComponentCount = 0;
}

void ComponentManager::MapComponent( size_t aEntityID, ComponentID aComponentID, size_t aComponentType )
{



}
