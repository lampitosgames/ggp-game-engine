#ifndef GGP_VERTEX_H
#define GGP_VERTEX_H

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex {
	DirectX::XMFLOAT3 position;	    // The position of the vertex
	DirectX::XMFLOAT2 uv;           // UV Coordinates
	DirectX::XMFLOAT3 normal;       // Normal vector
	DirectX::XMFLOAT3 tangent;      // Tangent vector
};

#endif //GGP_VERTEX_H