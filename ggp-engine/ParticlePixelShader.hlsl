#include "InputOutputStructs.hlsli"

cbuffer externalData : register(b0) {
	bool hasTexture;
}

Texture2D partTexture    : register(t0);
SamplerState trilinear   : register(s0);

float4 main(ParticleVertexToPixel input) : SV_TARGET{
	if (hasTexture) {
		input.color = partTexture.Sample(trilinear, input.uv) * input.color;
	}

	return input.color * input.color.a;
}