#include "VertexShaderGlobals.hlsli"
#include "InputOutputStructs.hlsli"

ParticleVertexToPixel main(ParticleVertexShaderInput input) {
	// Set up output struct
	ParticleVertexToPixel output;
	output.position = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.uv = float2(0.0f, 0.0f);
	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}