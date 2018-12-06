#include "stdafx.h"

#include "Collisions.h"

using namespace Physics;
using namespace DirectX;

const bool Physics::Collisions::IsPointInside(const DirectX::XMFLOAT3 & aPoint, const SphereCollider & aSphere) {
	float distance =
		static_cast<float>(sqrt(
		(aPoint.x - aSphere.Center.x) * (aPoint.x - aSphere.Center.x) +
			(aPoint.y - aSphere.Center.y) * (aPoint.y - aSphere.Center.y) +
			(aPoint.z - aSphere.Center.z) * (aPoint.z - aSphere.Center.z)));

	return distance < aSphere.Radius;
}

const bool Physics::Collisions::Intersects(const SphereCollider & aSphere, const SphereCollider & bSphere) {
	// we are using multiplications because it's faster than calling Math.pow
	float distance = static_cast<float>(sqrt(
		(aSphere.Center.x - bSphere.Center.x) * (aSphere.Center.x - bSphere.Center.x) +
		(aSphere.Center.y - bSphere.Center.y) * (aSphere.Center.y - bSphere.Center.y) +
		(aSphere.Center.z - bSphere.Center.z) * (aSphere.Center.z - bSphere.Center.z)));

	return distance < (aSphere.Radius + bSphere.Radius);
}

// got this from the following book: 
// http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
const int Physics::Collisions::IntersectRaySphere(Point p, const DirectX::XMFLOAT3 dFloat3, const SphereCollider & s, float & t, Point & q) {
	XMFLOAT3 mFloat3 = {};
	mFloat3.x = p.x - s.Center.x;
	mFloat3.y = p.y - s.Center.y;
	mFloat3.z = p.z - s.Center.z;

	XMVECTOR m = XMLoadFloat3(&mFloat3);
	XMVECTOR d = XMLoadFloat3(&dFloat3);
	XMVECTOR dotResult = DirectX::XMVector3Dot(m, d);

	float b = 0.f;  // Dot(m, d)
	XMStoreFloat(&b, dotResult);

	float c = 0.f;
	dotResult = DirectX::XMVector3Dot(m, m);
	XMStoreFloat(&c, dotResult);
	c -= (s.Radius * s.Radius);  // Dot(m,m) - s.r * s.r

	// Exit if r's origin is outside of s
	if (c > 0.0f && b > 0.0f) return 0;

	float discr = b * b - c;
	// A negative discriminant corresponds to a ray missing sphere
	if (discr < 0.0f) return 0;

	t = (float)(-b - sqrt(discr));
	// If t i snegative, then the ray started inside the sphere so clamp it to 0
	if (t < 0.0f) t = 0.0f;

	XMFLOAT3 r = {};
	XMStoreFloat3(&r, d);

	//q = p + t * d;
	q.x = p.x + t * r.x;
	q.y = p.y + t * r.y;
	q.z = p.z + t * r.z;

	return 1;
}

