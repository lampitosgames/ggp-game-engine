#include "PhysicsManager.h"
#include "RigidBody.h"
#include "Spatial.h"

using namespace DirectX;

PhysicsManager* PhysicsManager::Instance = nullptr;

const UINT PhysicsManager::GetRigidBodyCount() const
{
    return rBodyCount;
}

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
    RigidBody* rb = new RigidBody( aGameObj, aMass, aLayer );
    
    RigidBodyUIDMap.insert( std::pair<UINT, RigidBody*>( rBodyCount, rb ) );
    
    return rBodyCount++;
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

/** Iteration over a map is o(n), but it is not optimal for caching 
because the data is not stored contiguously. */
void PhysicsManager::UpdatePhysics( float deltaTime )
{
    RigidBody* entityA = nullptr;

    for ( auto itr = RigidBodyUIDMap.begin(); itr != RigidBodyUIDMap.end(); ++itr )
    {
        entityA = itr->second;

        if ( entityA->GetPhysicsLayer() == EPhysicsLayer::STATIC ) continue;

        XMFLOAT3 forceToApply = XMFLOAT3( 0.f, 0.f, 0.f );
        forceToApply.y += Gravity;

        forceToApply.x *= deltaTime;
        forceToApply.y *= deltaTime;
        forceToApply.z *= deltaTime;

        entityA->ApplyForce( forceToApply );

         

        // Check the floor pos        
        if ( entityA->gameObject->transform.position.y < -2.f )
        {
            XMFLOAT3 opposingVel = entityA->GetVelocity();
            opposingVel.x *= -1.f;
            opposingVel.y *= -1.f;
            opposingVel.z *= -1.f;
            entityA->ApplyForce( opposingVel );
        }
        entityA->ApplyAcceleration();

    }

}
