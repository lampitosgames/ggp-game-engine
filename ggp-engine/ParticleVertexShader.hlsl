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

	float normalizedTime = map(input.remainLife, 0.0f, input.startLife, 0.0f, 1.0f);
	output.color = input.startColor * normalizedTime + input.endColor * (1 - normalizedTime);
	float size = input.startSize * normalizedTime + input.endSize * (1 - normalizedTime);

	float2 offset = input.uv * 2 - 1;
	offset.y *= -1;
	position.xy += offset * size;

	matrix viewProj = mul(view, projection);
	output.position = mul(float4(position, 1.0f), viewProj);

	output.uv = input.uv;
	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}