#ifndef GGP_VERTEX_H
#define GGP_VERTEX_H

#include <SimpleMath.h>

struct Vertex {
	DirectX::SimpleMath::Vector3 position;	   // The position of the vertex
	DirectX::SimpleMath::Vector2 uv;           // UV Coordinates
	DirectX::SimpleMath::Vector3 normal;       // Normal vector
	DirectX::SimpleMath::Vector3 tangent;      // Tangent vector
};

#endif //GGP_VERTEX_H