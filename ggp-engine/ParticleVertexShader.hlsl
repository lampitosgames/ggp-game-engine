#include "VertexShaderGlobals.hlsli"
#include "InputOutputStructs.hlsli"

float map(float _v, float min1, float max1, float min2, float max2) {
	float perc = (_v - min1) / (max1 - min1);
	return perc * (max2 - min2) + min2;
}

ParticleVertexToPixel main(ParticleVertexShaderInput input) {
	// Set up output struct
	ParticleVertexToPixel output;

	//Calculate how long this particle has been alive
	float time = input.startLife - input.remainLife;

	//Calculate position using kinematics and constant acceleration
	float3 position = input.iPos + input.iVel * time + 0.5 * input.accel * time * time;

	float normalizedTime = map(input.remainLife, 0.0f, input.startLife, 0.0f, 1.0f);
	float size = input.startSize * normalizedTime + input.endSize * (1 - normalizedTime);

	//Calculate the offset based on the current UV (UV is the only per-vertex variable. Everything else is per-instance)
	float2 offset = input.uv * 2 - 1;
	offset.y *= -1;

	//Calculate rotation. Rotate in UV space before transforming into world space
	float curRot = input.iRot + input.angularVel * time;
	float2x2 rotMat = { cos(curRot), -sin(curRot), sin(curRot), cos(curRot) };
	offset = mul(offset, rotMat);

	//Get camera-space vectors for billboarding
	float3 camRight = float3(view[0][0], view[1][0], view[2][0]);
	float3 camUp = float3(view[0][1], view[1][1], view[2][1]);

	//Calculate the final world-space position of the vertex using the camera's local axes, the offset, and the particle's current size
	float3 vertOffset = camRight * offset.x * size + camUp * offset.y * size;
	position += vertOffset;

	//Transform the world space position into screen space
	output.position = mul(float4(position, 1.0f), mul(view, projection));

	//Pass UV coords to pixel shader
	output.uv = input.uv;

	//Lerp color based on time. Pass to pixel shader
	output.color = input.startColor * normalizedTime + input.endColor * (1 - normalizedTime);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}