#include "PhysicsManager.h"
#include "RigidBody.h"
#include "Spatial.h"

PhysicsManager* PhysicsManager::Instance = nullptr;

PhysicsManager::PhysicsManager()
{

}

PhysicsManager::~PhysicsManager()
{
    for ( auto itr = RigidBodyUIDMap.begin(); itr != RigidBodyUIDMap.end(); ++itr )
    {
        RigidBody* temp = itr->second;
        if ( temp != nullptr )
        {
            delete temp;
        }
    }
}

PhysicsManager* PhysicsManager::GetInstance()
{
    if ( Instance == nullptr )
    {
        Instance = new PhysicsManager();
    }
    return Instance;
}

void PhysicsManager::ReleaseInstance()
{
    if ( Instance != nullptr )
    {
        delete Instance;
        Instance = nullptr;
    }
}

UINT PhysicsManager::AddRigidBody( Spatial* aGameObj, float aMass, EPhysicsLayer aLayer )
{
    assert( aGameObj );
    RigidBody* rb = new RigidBody( aMass, aLayer );
    rb->gameObject = aGameObj;
    RigidBodyUIDMap[ rBodyCount ] = rb;

    ++rBodyCount;
    return rBodyCount - 1;
}

RigidBody * PhysicsManager::GetRigidBody( UINT uID )
{
    auto thisRB = RigidBodyUIDMap.find( uID );
    //If found, return it.  Else, return nullptr
    if ( thisRB != RigidBodyUIDMap.end() ) {
        return thisRB->second;
    }
    return nullptr;
}
