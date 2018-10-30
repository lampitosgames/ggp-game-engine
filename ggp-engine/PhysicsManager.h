#pragma once

#include <map>

////////////////////////////
// Forward Declarations
class RigidBody;
class Spatial;
enum EPhysicsLayer;

typedef unsigned int UINT;

class PhysicsManager
{
public:

    static PhysicsManager * GetInstance();

    static void ReleaseInstance();

    UINT AddRigidBody( Spatial* aGameObj, float aMass, EPhysicsLayer aLayer );

    RigidBody* GetRigidBody( UINT uID );

    
    // We don't want anything making copies of this class so delete these operators
    PhysicsManager( PhysicsManager const& ) = delete;
    void operator=( PhysicsManager const& ) = delete;

private:

    static PhysicsManager * Instance;

    PhysicsManager();
    ~PhysicsManager();



    std::map<UINT, RigidBody*> RigidBodyUIDMap;
    
    UINT rBodyCount;

};

