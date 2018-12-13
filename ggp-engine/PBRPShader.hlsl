#include "PBRShaderGlobals.hlsli"
#include "LightGlobals.hlsli"
#include "InputOutputStructs.hlsli"

//For depth of field
cbuffer externalDate : register(b5)
{
	float4 dofPara; // Depth of Field Paramaters
}
//TEXTURE REGISTERS
SamplerState basicSampler : register(s0);
Texture2D albedoTexture   : register(t0); //needs gamma correction
Texture2D normalMap       : register(t1); //No gamma correction
Texture2D roughnessMap    : register(t2); //no gamma correction
Texture2D metalnessMap    : register(t3); //needs gamma correction

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
	float4 albedo = baseColor;
	//If there is a diffuse texture, use the texture color instead of the baseColor
	if (useAlbedoTex == 1) {
		albedo = albedoTexture.Sample(basicSampler, input.uv);
		//Gamma corrections
		albedo = pow(albedo, gammaModifier);
	}
	//Grab roughness
	float roughness = baseRoughness;
	if (useRoughnessTex == 1) { roughness = roughnessMap.Sample(basicSampler, input.uv).r; }
	//Grab metalness
	float metalness = baseMetalness;
	if (useMetalnessTex == 1) {
		metalness = metalnessMap.Sample(basicSampler, input.uv).r;
		metalness = pow(metalness, gammaModifier);
	}

	//Get a specular value from the albedo texture.
	//Then, interpolate between the fresnel value for dielectrics and the full specular 
	//color based on the metalness at this pixel. This is the F0 variable in the fresnel equation
	float3 specColor = lerp(DIELECTRIC_FRESNEL_CONST.rrr, albedo.rgb, metalness);

	//Variable to store summed light strength for this pixel
	float3 lightColorSum = float3(0.0f, 0.0f, 0.0f);
	//Loop through direcitonal lights
	for (uint i = 0; i < maxDirLightCount; i++) {
		if (i >= dirLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcDirLightPBR(dirLights[i], input.normal, cameraPosition, input.worldPos, albedo, roughness, metalness, specColor);
	}
	//Loop through point lights
	for (uint j = 0; j < maxPointLightCount; j++) {
		if (j >= pointLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcPointLightPBR(pointLights[j], input.normal, cameraPosition, input.worldPos, albedo, roughness, metalness, specColor);
	}
	//Loop through spot lights
	for (uint k = 0; k < maxSpotLightCount; k++) {
		if (k >= spotLightCount) { break; }
		//Add each light's calculated value to the total color sum
		lightColorSum += calcSpotLightPBR(spotLights[k], input.normal, cameraPosition, input.worldPos, albedo, roughness, metalness, specColor);
	}
	//Gamma correction
	float3 gammaCorrect = pow(lightColorSum, 1.0f / gammaModifier);

	//Return the mesh's surface color multiplied by the calculated light data
	output.color = float4(gammaCorrect, 1.0f);

	//Calculate depth and output to depth render target
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