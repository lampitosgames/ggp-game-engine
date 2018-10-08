#include "PixelShaderGlobals.hlsli"
#include "LightGlobals.hlsli"
#include "TextureGlobals.hlsli"
#include "InputOutputStructs.hlsli"

float4 main(VertexToPixel input) : SV_TARGET {
	//Normalize incoming normal vector
	input.normal = normalize(input.normal);
    //Get the direction vector to the camera (needed for specular)
    float3 dirToCamera = normalize(cameraPosition - input.worldPos);

	//Calculate lighting for directional lights
	float4 dirLightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (uint i = 0; i < 2; i++) {
		if (i >= dirLightCount) { break; }
		//Diffuse 
		float NdotL = dot(input.normal, normalize(-dirLights[i].direction));
		//Specular
		float3 h = normalize(-dirLights[i].direction + dirToCamera);
		float NdotH = saturate(dot(input.normal, h));
		float specAmt = pow(NdotH, baseSpec);

		dirLightColorSum += dirLights[i].ambientColor + (NdotL * dirLights[i].diffuseColor) + float4(specAmt, specAmt, specAmt, 1.0f);
	}

	float4 pointLightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

	[unroll(6)]
	for (uint j = 0; j < 6; i++) {
		if (j >= pointLightCount) { break; }
		//Calculate direction to light
		float3 lightDir = pointLights[j].position - input.worldPos;
		float4 lightDist = length(lightDir);
		float lightAtten = 1.0f / (pointLights[j].constAtten + pointLights[j].linearAtten*lightDist + pointLights[j].expAtten*lightDist*lightDist);
		//Diffuse
		float NdotL = dot(input.normal, normalize(lightDir));
		//Specular
		float3 h = normalize(-lightDir + dirToCamera);
		float NdotH = saturate(dot(input.normal, h));
		float specAmt = pow(NdotH, baseSpec);

		pointLightColorSum += pointLights[j].ambientColor * pointLights[j].color + lightAtten * (NdotL * pointLights[j].color*pointLights[j].diffuseColor) + lightAtten*float4(specAmt, specAmt, specAmt, 1.0f);
	}

	return saturate(dirLightColorSum + pointLightColorSum);
}