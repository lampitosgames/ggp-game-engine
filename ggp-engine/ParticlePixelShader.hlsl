#include "InputOutputStructs.hlsli"

Texture2D partTexture    : register(t0);
SamplerState trilinear   : register(s0);

float4 main(ParticleVertexToPixel input) : SV_TARGET{
	return partTexture.Sample(trilinear, input.uv) * input.color * input.color.a;
}