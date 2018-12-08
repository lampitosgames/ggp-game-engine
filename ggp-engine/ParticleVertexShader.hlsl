#include "VertexShaderGlobals.hlsli"
#include "InputOutputStructs.hlsli"

ParticleVertexToPixel main(ParticleVertexShaderInput input, uint instanceID : SV_InstanceID) {
	// Set up output struct
	ParticleVertexToPixel output;
	matrix viewProj = mul(view, projection);
	output.position = mul(float4(input.iPos, 1.0f), viewProj);

	float2 offset = input.uv * 2 - 1;
	offset.y *= -1;
	offset *= 0.3;
	output.position.xy += offset;
	output.position.y += instanceID;
	output.uv = input.uv;
	output.color = input.startColor;
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}