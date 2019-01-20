#include "PShaderGlobals.hlsli"
#include "LightGlobals.hlsli"
#include "InputOutputStructs.hlsli"

//For depth of field
cbuffer externalDate : register(b5)
{
	float4 dofPara; // Depth of Field Paramaters
}

//TEXTURE REGISTERS
SamplerState basicSampler : register(s0);
Texture2D diffuseTexture  : register(t0);
Texture2D normalMap       : register(t1);
Texture2D specularMap     : register(t2);

//Entry point
PixelOut main(VertexToPixel input) {
	PixelOut output;
	//Re-normalize the lerped coordinate vectors
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	//If there is a normal map, calculate a new normal from the normal map
	if (useNormalTex == 1) {
		//Get normal map from texture
		float3 mapNormal = normalMap.Sample(basicSampler, input.uv).rgb * 2 - 1;
		input.normal = applyNormalMap(input.normal, input.tangent, mapNormal);
	}

	//Grab the surface color
	float4 surfaceColor = baseColor;
	//If there is a diffuse texture, use the texture color instead of the baseColor
	if (useDiffuseTex == 1) { 
		surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
		//Gamma corrections
		surfaceColor = pow(surfaceColor, gammaModifier);
	}
	//Grab shininess
	float shininess = baseSpec;
	if (useSpecularTex == 1) { shininess = specularMap.Sample(basicSampler, input.uv).r; }

	//Variable to store summed light strength for this pixel
	float3 lightColorSum = float3(0.0f, 0.0f, 0.0f);
	//Loop through direcitonal lights
	for (uint i = 0; i < maxDirLightCount; i++) {
		if (i >= dirLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcDirLight(dirLights[i], input.normal, surfaceColor, cameraPosition, input.worldPos, shininess);
	}
	//Loop through point lights
	for (uint j = 0; j < maxPointLightCount; j++) {
		if (j >= pointLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcPointLight(pointLights[j], input.normal, surfaceColor, cameraPosition, input.worldPos, shininess);
	}
	//Loop through spot lights
	for (uint k = 0; k < maxSpotLightCount; k++) {
		if (k >= spotLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcSpotLight(spotLights[j], input.normal, surfaceColor, cameraPosition, input.worldPos, shininess);
	}

	//Gamma correction
	float3 gammaCorrect = pow(lightColorSum, 1.0f / gammaModifier);

	//Return the mesh's surface color multiplied by the calculated light data
	output.color = saturate(float4(gammaCorrect, 1.0f));

	//Calculate the depth based on near, focus and far
	float f = 0.0f;
	if (input.depth < dofPara.y)
	{
		f = (input.depth - dofPara.y) / (dofPara.y - dofPara.x);
	}
	else
	{
		f = (input.depth - dofPara.y) / (dofPara.z - dofPara.y);
		f = clamp(f, 0, dofPara.w);
	}
	output.distance = float4((f*0.5f + 0.5f), 0, 0, 0);

	return output;
}