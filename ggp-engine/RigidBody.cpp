#include "RigidBody.h"
#include "Spatial.h"

using namespace DirectX;

RigidBody::RigidBody()
{
    Mass = 1.f;
    PhysicsLayer = EPhysicsLayer::STATIC;
    Acceleration = XMFLOAT3( 0.f, 0.f, 0.f );
    Velocity = XMFLOAT3( 0.f, 0.f, 0.f );

}

RigidBody::RigidBody( float aMass, EPhysicsLayer aPhysicsLayer )
    : Mass( aMass ), PhysicsLayer( aPhysicsLayer )

{
    Acceleration = XMFLOAT3( 0.f, 0.f, 0.f );
    Velocity = XMFLOAT3( 0.f, 0.f, 0.f );
}

RigidBody::~RigidBody()
{

}

void RigidBody::ApplyForce( const DirectX::XMFLOAT3 & aForce )
{
    XMVECTOR force = XMLoadFloat3( &aForce );
    // Scale the vector by the mass
    force /= Mass;

    // Add to position and store
    XMStoreFloat3(
        &Acceleration,
        XMLoadFloat3( &Acceleration ) + force );
}

void RigidBody::ApplyAcceleration()
{
    XMVECTOR curAcceleration = XMLoadFloat3( &Acceleration );

    // Apply the acceleration to the velocity
    XMStoreFloat3(
        &Velocity,
        XMLoadFloat3( &Velocity ) + curAcceleration );

    // Add to position and store
    XMStoreFloat3(
        &gameObject->transform.position,
        XMLoadFloat3( &gameObject->transform.position ) + XMLoadFloat3( &Velocity ) );
        
    XMStoreFloat3( &Acceleration, curAcceleration * 0.f );
}

////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////

const EPhysicsLayer RigidBody::GetPhysicsLayer() const
{
    return PhysicsLayer;
}

void RigidBody::SetPhysicsLayer( EPhysicsLayer aLayer )
{
    PhysicsLayer = aLayer;
}

void RigidBody::SetVelocity( const DirectX::XMFLOAT3 & aVel )
{
    Velocity = aVel;
}

const DirectX::XMFLOAT3 & RigidBody::GetVelocity() const
{
    return Velocity;
}

const float RigidBody::GetMass() const
{
    return Mass;
}

void RigidBody::SetMass( float aMass )
{
    Mass = aMass;
}