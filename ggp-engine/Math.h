#ifndef GGP_MATH_H
#define GGP_MATH_H

#include <DirectXMath.h>

/*
	Eventually this will be a wrapper library for DirectX math types
	For now, it is just a list of typedefs that make the types consistent
	with GLSL
*/


typedef unsigned int UINT;
typedef DirectX::XMFLOAT2 float2;
typedef DirectX::XMFLOAT3 float3;
typedef DirectX::XMFLOAT4 float4;


#endif //GGP_MATH_H