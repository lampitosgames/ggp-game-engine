#include "VertexShaderGlobals.hlsli"
#include "InputOutputStructs.hlsli"

VertexToPixel main(VertexShaderInput input) {
	// Set up output struct
	VertexToPixel output;

	//Create world space -> screen space transformation matrix
	matrix worldViewProj = mul(mul(world, view), projection);

	//Transform vertex position into screen space (used for drawing pixels to the screen)
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	//Transform vertex position into world space (used for calculations further down the pipeline)
	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	//Depth in view space
	output.depth = mul(float4(input.position, 1.0f), mul(world, view)).z;

	//Store output normal and tangent vectors. Transform them using the inverse transpose of the world matrix
	output.normal = mul(input.normal, (float3x3)worldInvTrans);
	output.tangent = mul(input.tangent, (float3x3)worldInvTrans);

	output.uv = input.uv;
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}