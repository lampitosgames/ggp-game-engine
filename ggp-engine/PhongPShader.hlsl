#include "LightStructs.hlsli"
#include "InputOutputStructs.hlsli"

cbuffer externalData : register(b0) {
	float3 cameraPosition;
	//Material properties
	float4 color;
	float shininess;
}

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
    //Get the direction vector to the camera (needed for specular)
    float3 dirToCamera = normalize(cameraPosition - input.worldPos);

	//Calculate lighting for directional lights
	float4 dirLightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (uint i = 0; i < 4; i++) {
		if (i >= dirLightCount) { break; }
		//Diffuse 
		float NdotL = dot(input.normal, normalize(-dirLights[i].direction));
		//Specular
		float3 h = normalize(-dirLights[i].direction + dirToCamera);
		float NdotH = saturate(dot(input.normal, h));
		float specAmt = pow(NdotH, shininess);

		dirLightColorSum += dirLights[i].ambientColor + (NdotL * dirLights[i].diffuseColor) + float4(specAmt, specAmt, specAmt, 1.0f);
	}

	float4 pointLightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 4; i++) {
		if (i >= pointLightCount) { break; }
		//Calculate direction to light
		float3 lightDir = pointLights[i].position - input.worldPos;
		float4 lightDist = length(lightDir);
		float lightAtten = 1.0f / (pointLights[i].constAtten + pointLights[i].linearAtten*lightDist + pointLights[i].expAtten*lightDist*lightDist);
		//Diffuse
		float NdotL = dot(input.normal, normalize(lightDir));
		//Specular
		float3 h = normalize(-lightDir + dirToCamera);
		float NdotH = saturate(dot(input.normal, h));
		float specAmt = pow(NdotH, shininess);

		pointLightColorSum += pointLights[i].ambientColor * pointLights[i].color + lightAtten * (NdotL * pointLights[i].color*pointLights[i].diffuseColor) + lightAtten*float4(specAmt, specAmt, specAmt, 1.0f);
	}

	return saturate(dirLightColorSum + pointLightColorSum);
}