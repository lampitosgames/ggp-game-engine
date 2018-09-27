#include "LightStructs.hlsli"
#include "InputOutputStructs.hlsli"

static const uint maxLightCount = 9;
cbuffer lightData : register(b0) {
	DirectionalLight dirLights[maxLightCount];
	uint dirLightCount;
}

float4 main(VertexToPixel input) : SV_TARGET {
	//Normalize incoming normal vector
	input.normal = normalize(input.normal);

	float4 dirLightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

	for (uint i = 0; i < dirLightCount; i++) {
		float NdotL = dot(input.normal, normalize(-dirLights[i].direction));
		dirLightColorSum += dirLights[i].ambientColor + (NdotL * dirLights[i].diffuseColor);
	}

	dirLightColorSum = saturate(dirLightColorSum);

	return dirLights[0].diffuseColor;
}