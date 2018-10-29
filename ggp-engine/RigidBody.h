#pragma once
#include "Component.h"
#include <DirectXMath.h>

/// <summary>
/// Rigidbody class for objects that will have basic physics on them
/// </summary>
/// <author>Ben Hoffman</author>
class RigidBody :
    public Component
{
public:
    
    
    RigidBody();

    RigidBody( float aMass );

    ~RigidBody();


    void ApplyForce( const DirectX::XMFLOAT3 aForce );


    void ApplyAcceleration();



private:

    float Mass;

    DirectX::XMFLOAT3 Acceleration;

    DirectX::XMFLOAT3 Velcoity;


};

