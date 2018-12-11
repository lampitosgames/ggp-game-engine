#include "stdafx.h"

#include "RigidBody.h"
#include "GameObject.h"

using namespace DirectX;


RigidBody::RigidBody(GameObject* aGameObj, float aMass, EPhysicsLayer aPhysicsLayer)
	: gameObject(aGameObj), Mass(aMass), PhysicsLayer(aPhysicsLayer)

{
	Acceleration = XMFLOAT3(0.f, 0.f, 0.f);
	Velocity = XMFLOAT3(0.f, 0.f, 0.f);

	Collider.Center = gameObject->transform.position;
	Collider.Radius = 0.2f;

	Physics::PhysicsManager::GetInstance()->AddRigidBody(this);
}

RigidBody::~RigidBody() {
	gameObject = nullptr;
	// remove from Physics Manager
	Physics::PhysicsManager::GetInstance()->RemoveRigidBody(this);
}

void RigidBody::ApplyForce(const DirectX::XMFLOAT3 & aForce) {
	XMVECTOR force = XMLoadFloat3(&aForce);
	// Scale the vector by the mass
	force /= Mass;

	// Add to position and store
	XMStoreFloat3(
		&Acceleration,
		XMLoadFloat3(&Acceleration) + force);
}

void RigidBody::ApplyAcceleration() {
	XMVECTOR curAcceleration = XMLoadFloat3(&Acceleration);

	// Apply the acceleration to the velocity
	XMStoreFloat3(
		&Velocity,
		XMLoadFloat3(&Velocity) + curAcceleration);

	// Add to position and store
	XMStoreFloat3(
		&gameObject->transform.position,
		XMLoadFloat3(&gameObject->transform.position) + XMLoadFloat3(&Velocity));

	XMStoreFloat3(&Acceleration, curAcceleration * 0.f);
}

////////////////////////////////////////////////////
// Accessors
////////////////////////////////////////////////////

const EPhysicsLayer RigidBody::GetPhysicsLayer() const {
	return PhysicsLayer;
}

void RigidBody::SetPhysicsLayer(EPhysicsLayer aLayer) {
	PhysicsLayer = aLayer;
}

void RigidBody::SetVelocity(const DirectX::XMFLOAT3 & aVel) {
	Velocity = aVel;
}

const DirectX::XMFLOAT3 & RigidBody::GetVelocity() const {
	return Velocity;
}

const float RigidBody::GetMass() const {
	return Mass;
}

void RigidBody::SetMass(float aMass) {
	Mass = aMass;
}

const Physics::SphereCollider & RigidBody::GetCollider() const {
	return Collider;
	// TODO: insert return statement here
}
