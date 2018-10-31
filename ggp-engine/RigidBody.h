#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include "Spatial.h"
#include "Collisions.h"

enum EPhysicsLayer
{
    MOVEABLE,
    STATIC
};

/// <summary>
/// Rigidbody class for objects that will have basic physics on them
/// </summary>
/// <author>Ben Hoffman</author>
class RigidBody :
    public Component
{
public:
    
    RigidBody( Spatial* aGameObj, float aMass, EPhysicsLayer aPhysicsLayer );

    ~RigidBody();

    /// <summary>
    /// Apply a force to this entity
    /// </summary>
    /// <param name="aForce">The force to apply</param>
    void ApplyForce( const DirectX::XMFLOAT3 & aForce );

    /// <summary>
    /// Apply the current acceleration to the velocity and set acceleration to 0
    /// </summary>
    void ApplyAcceleration();

    Spatial * gameObject; // :S
    
    /////////////////////////////////////////////////////////
    // Accessors

    const EPhysicsLayer GetPhysicsLayer() const;

    void SetPhysicsLayer( const EPhysicsLayer aLayer );

    void SetVelocity( const DirectX::XMFLOAT3& aVel );

    const DirectX::XMFLOAT3 & GetVelocity() const;

    const float GetMass() const;

    void SetMass( const float aMass );    

    const Physics::SphereCollider & GetCollider() const;

private:

    float Mass = 1.0f;

    DirectX::XMFLOAT3 Acceleration;

    DirectX::XMFLOAT3 Velocity;

    EPhysicsLayer PhysicsLayer;

    Physics::SphereCollider Collider;

};

