#include "LightStructs.hlsli"
#include "InputOutputStructs.hlsli"

//EXTERNAL DATA
cbuffer externalData : register(b0) {
	float3 cameraPosition;
	//Material properties
	float4 color;
	float shininess;
}

//TEXTURE REGISTERS
Texture2D diffuseTexture  : register(t0);
SamplerState basicSampler : register(s0);

//LIGHTING DATA
static const uint maxLightCount = 4;
cbuffer dirLightData : register(b1) {
	DirectionalLight dirLights[maxLightCount];
	uint dirLightCount;
}

cbuffer pointLightData : register(b2) {
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

	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	return surfaceColor * saturate(dirLightColorSum + pointLightColorSum);
}