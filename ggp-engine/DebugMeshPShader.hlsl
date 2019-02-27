#include "PShaderGlobals.hlsli"
#include "InputOutputStructs.hlsli"

//TEXTURE REGISTERS
SamplerState basicSampler : register(s0);
Texture2D diffuseTexture  : register(t0);
Texture2D normalMap       : register(t1);
Texture2D specularMap     : register(t2);

//Entry point
float4 main(VertexToPixel input) : SV_TARGET{
	//Gamma correction
	float3 gammaCorrect = pow(baseColor, 1.0f / gammaModifier);

	//Return the mesh's surface color multiplied by the calculated light data
	return saturate(float4(gammaCorrect, 1.0f));
}