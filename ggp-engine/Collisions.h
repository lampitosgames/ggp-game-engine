#pragma once

#include <DirectXMath.h>

namespace Physics {

	/// <summary>
	/// A simple sphere collider
	/// </summary>
	struct SphereCollider {
		float Radius;
		DirectX::XMFLOAT3 Center;
	};

	struct Point {
		float x, y, z;
	};

	/// <summary>
	/// Static class with collision calculations and other helpful
	/// physics definitions
	/// </summary>
	/// <author>Ben Hoffman</author>
	class Collisions {
	public:

		/// <summary>
		/// Check if the point of interest is within the bounds of 
		/// the given sphere
		/// </summary>
		/// <param name="aPoint">Point of interest</param>
		/// <param name="aSphere">Sphere collider to check against</param>
		/// <returns>True if the point is inside of the sphere's radius</returns>
		static const bool IsPointInside(const DirectX::XMFLOAT3 & aPoint, const SphereCollider & aSphere);

		/// <summary>
		/// Checks if two sphere colliders are intersecting 
		/// </summary>
		/// <param name="aSphere">First sphere of interest</param>
		/// <param name="bSphere">Second sphere of interest</param>
		/// <returns>true if these two spheres are intersecting</returns>
		static const bool Intersects(const SphereCollider & aSphere, const SphereCollider & bSphere);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="p">Ray origin</param>
		/// <param name="d">Normalized directionaly vector</param>
		/// <param name="s">Sphere to check</param>
		/// <param name="t"></param>
		/// <param name="q"></param>
		/// <returns></returns>
		static const int IntersectRaySphere(Point p, const DirectX::XMFLOAT3 d, const SphereCollider & s, float &t, Point &q);

	};

}
