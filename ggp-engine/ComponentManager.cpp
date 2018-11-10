#include "stdafx.h"
#include "ComponentManager.h"
#include "GameObject.h"

// Singleton requirement
//ECS::ComponentManager* instance = nullptr;
ECS::ComponentManager* ECS::ComponentManager::instance = nullptr;
using namespace ECS;

ECS::ComponentManager* ECS::ComponentManager::GetInstance()
{
    if ( instance == nullptr )
    {
        instance = new ComponentManager();
    }
    return instance;
}

void ECS::ComponentManager::ReleaseInstance()
{
    if ( instance != nullptr )
    {
        delete instance;
        instance = nullptr;
    }
}

ECS::ComponentManager::ComponentManager()
{
}

ECS::ComponentManager::~ComponentManager()
{
    CleanupAllComponents();
}

void ECS::ComponentManager::CleanupAllComponents()
{
   /* auto map_itr = activeComponents.begin();

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
    }*/
    ComponentCount = 0;
}

void ECS::ComponentManager::MapComponent( size_t aEntityID, ComponentID aComponentID, ComponentTypeId aComponentType )
{


}
