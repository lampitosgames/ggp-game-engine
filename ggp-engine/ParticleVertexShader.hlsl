#include "VertexShaderGlobals.hlsli"
#include "InputOutputStructs.hlsli"

float map(float _v, float min1, float max1, float min2, float max2) {
	float perc = (_v - min1) / (max1 - min1);
	return perc * (max2 - min2) + min2;
}

ParticleVertexToPixel main(ParticleVertexShaderInput input) {
	// Set up output struct
	ParticleVertexToPixel output;

	float time = input.startLife - input.remainLife;
	//Calculate position using kinematics and constant acceleration
	float3 position = input.iPos + input.iVel * time + 0.5 * input.accel * time * time;


	float2 offset = input.uv * 2 - 1;
	offset.y *= -1;
	position.xy += offset;

	matrix viewProj = mul(view, projection);
	output.position = mul(float4(position, 1.0f), viewProj);

	output.uv = input.uv;
	output.color = float4(map(input.remainLife, 0.0f, 10.0f, 0.0f, 1.0f), 0.0f, 0.0f, 1.0f);
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}