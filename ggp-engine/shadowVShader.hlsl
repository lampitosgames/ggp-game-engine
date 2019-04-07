#include "VertexShaderGlobals.hlsli"
#include "InputOutputStructs.hlsli"

ShadowToPixel main(VertexShaderInput input) {
	// Set up output struct
	ShadowToPixel output;
	//Calculate output position using model world and view with the light's projection
	matrix wvp = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), wvp);
	return output;
}