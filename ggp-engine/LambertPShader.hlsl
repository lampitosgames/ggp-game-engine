#include "LightStructs.hlsli"
#include "InputOutputStructs.hlsli"

static const uint maxLightCount = 4;
cbuffer dirLightData : register(b0) {
	DirectionalLight dirLights[maxLightCount];
	uint dirLightCount;
}

cbuffer pointLightData : register(b1) {
	PointLight pointLights[maxLightCount];
	uint pointLightCount;
}

float4 main(VertexToPixel input) : SV_TARGET {
	//Normalize incoming normal vector
	input.normal = normalize(input.normal);

	float4 dirLightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 4; i++) {
		if (i >= dirLightCount) { break; }
		float NdotL = dot(input.normal, normalize(-dirLights[i].direction));
		dirLightColorSum += dirLights[i].ambientColor + (NdotL * dirLights[i].diffuseColor);
	}

	float4 pointLightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 4; i++) {
		if (i >= pointLightCount) { break; }
		float3 lightDir = pointLights[i].position - input.worldPos;
		float4 lightDist = length(lightDir);
		float lightAtten = 1.0f / (pointLights[i].constAtten + pointLights[i].linearAtten*lightDist + pointLights[i].expAtten*lightDist*lightDist);
		float NdotL = dot(input.normal, normalize(lightDir));
		pointLightColorSum += pointLights[i].ambientColor * pointLights[i].color + lightAtten * (NdotL * pointLights[i].color*pointLights[i].diffuseColor);
	}

	return saturate(dirLightColorSum + pointLightColorSum);
}