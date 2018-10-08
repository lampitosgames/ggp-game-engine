#include "PixelShaderGlobals.hlsli"
#include "LightGlobals.hlsli"
#include "TextureGlobals.hlsli"
#include "InputOutputStructs.hlsli"

//LIGHTING FUNCTIONS
float4 calcDirLight(float3 _normal, float3 _lightDir, float4 _diffuse, float4 _ambient) {
	float NdotL = dot(_normal, normalize(-_lightDir));
	return saturate(_ambient + (NdotL * _diffuse));
}

float4 calcPointLight(float3 _normal, float3 _pixWorldPos, PointLight _light) {
	//Get direction from the lit pixel to the point light
	float3 lightDir = _light.position - _pixWorldPos;
	//Distance to the light
	float4 lightDist = length(lightDir);
	//Attenuation calculation
	float lightAtten = 1.0f / (_light.constAtten + _light.linearAtten*lightDist + _light.expAtten*lightDist*lightDist);
	//Lambert "N * L" calculation
	float NdotL = dot(_normal, normalize(lightDir));
	//Store diffuse and ambient colors
	float4 diffuseColor = _light.diffuseColor * _light.color;
	float4 ambientColor = _light.ambientColor * _light.color;
	//Return the final effect of the light on this pixel
	return saturate(ambientColor + lightAtten * (NdotL * diffuseColor));
}

//Entry point
float4 main(VertexToPixel input) : SV_TARGET {
	//Re-normalize the lerped coordinate vectors
	input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
	//If there is a normal map, modify the normal vector
	float3 pixNormal;
    if (texChanToggle.y == 1) {
		//Get normal map from texture
		float3 mapNormal = normalMap.Sample(basicSampler, input.uv).rgb * 2 - 1;
		//Construct TBN matrix
		//NOTE: I followed the demo on mycourses for this (Game Graphics Programming)
		//Normal
		float3 N = input.normal;
		//Tangent
		float3 T = normalize(input.tangent - N * dot(input.tangent, N));
		//Bi-tangent
		float3 B = cross(T, N);
		//TBN Matrix
		float3x3 TBN = float3x3(T, B, N);
		//Adjust normal vector using the data from the normal map
		//Transform the normal map vector into local pixel space
		input.normal = normalize(mul(mapNormal, TBN));
	}

    //Variable to store summed light strength on this pixel
	float4 lightColorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//Loop through direcitonal lights
	for (uint i = 0; i < maxDirLightCount; i++) {
		if (i >= dirLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcDirLight(input.normal, dirLights[i].direction, dirLights[i].diffuseColor, dirLights[i].ambientColor);
	}
	//Loop through point lights
	for (uint j = 0; j < maxPointLightCount; j++) {
		if (j >= pointLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcPointLight(input.normal, input.worldPos, pointLights[j]);
	}
	
	//If there is a diffuse texture, use it
	float4 surfaceColor;
	if (texChanToggle.x == 1) {
		surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
	//Else, just use the material color
	} else {
		surfaceColor = baseColor;
	}
	
	//Return the mesh's surface color multiplied by lighting
	return surfaceColor * saturate(lightColorSum);
}