#ifndef GGP_VERTEX_H
#define GGP_VERTEX_H

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex {
	DirectX::XMFLOAT3 Position;	    // The position of the vertex
	DirectX::XMFLOAT4 Color;        // The color of the vertex
};

#endif //GGP_VERTEX_H