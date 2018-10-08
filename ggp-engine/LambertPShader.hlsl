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
	//If there is a normal map, calculate a new normal from the normal map
    if (texChanToggle.y == 1) {
		input.normal = applyNormalMap(input.normal, input.tangent, input.uv);
	}

    //Variable to store summed light strength for this pixel
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
	
	//Grab the surface color
	float4 surfaceColor = baseColor;
	//If there is a diffuse texture, use the texture color instead of the baseColor
	if (texChanToggle.x == 1) {
		surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
	}
	
	//Return the mesh's surface color multiplied by the calculated light data
	return surfaceColor * saturate(lightColorSum);
}