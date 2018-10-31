#pragma once

#include <map>

////////////////////////////
// Forward Declarations
class RigidBody;
class Spatial;
enum EPhysicsLayer;

namespace Physics
{
    typedef unsigned int UINT;

    /// <summary>
    /// Control the regular updating of physics on nay rigid body objects
    /// This is a very minimalistic approach to physics, because this project is 
    /// supposed to showcase different graphics techniques above gameplay.
    /// </summary>
    class PhysicsManager
    {
    public:

        /// <summary>
        /// Get a static instance to the physic manager
        /// </summary>
        /// <returns>Static pointer to the physics manager</returns>
        static PhysicsManager * GetInstance();

        /// <summary>
        /// Release the physics manager instance
        /// </summary>
        static void ReleaseInstance();

        /// <summary>
        /// Add a rigid body to the physics manager
        /// </summary>
        /// <param name="aGameObj">The spatial object that has this rigidbody</param>
        /// <param name="aMass">The mass of this rigidbody</param>
        /// <param name="aLayer">Physics layer of this rigidbody</param>
        /// <returns>Unique ID of this rigid body</returns>
        UINT AddRigidBody( Spatial* aGameObj, float aMass, EPhysicsLayer aLayer );

        /// <summary>
        /// Get a rigidbody based on it's ID
        /// </summary>
        /// <param name="uID">ID to look for</param>
        /// <returns>Pointer to that rigidbody</returns>
        RigidBody* GetRigidBody( UINT uID );

        /// <summary>
        /// Update the physics on all rigidbodies
        /// </summary>
        /// <param name="dt"></param>
        void UpdatePhysics( float dt );

        /// <summary>
        /// Get the current number of rigid bodies
        /// </summary>
        /// <returns>const UINT representing the number of bodies</returns>
        const UINT GetRigidBodyCount() const;

        // We don't want anything making copies of this class so delete these operators
        PhysicsManager( PhysicsManager const& ) = delete;
        void operator=( PhysicsManager const& ) = delete;

    private:

        PhysicsManager();

        ~PhysicsManager();

        static PhysicsManager * Instance;

        const float Gravity = -0.001f;

        // #Optimize
        std::map<UINT, RigidBody*> RigidBodyUIDMap;

        UINT rBodyCount;

    };


}
