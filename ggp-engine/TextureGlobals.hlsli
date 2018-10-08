//For use in all pixel shaders. Provides bindings for textures

//TEXTURE REGISTERS
SamplerState basicSampler : register(s0);
Texture2D diffuseTexture  : register(t0);
Texture2D normalMap       : register(t1);
Texture2D specularMap     : register(t2);

//FUNCTIONS
float3 applyNormalMap(float3 _normal, float3 _tangent, float2 _uv) {
	//Get normal map from texture
	float3 mapNormal = normalMap.Sample(basicSampler, _uv).rgb * 2 - 1;
	//Construct TBN matrix
	//NOTE: I followed the demo on mycourses for this (Game Graphics Programming)
	//Normal
	float3 N = _normal;
	//Tangent
	float3 T = normalize(_tangent - N * dot(_tangent, N));
	//Bi-tangent
	float3 B = cross(T, N);
	//TBN Matrix
	float3x3 TBN = float3x3(T, B, N);
	//Adjust normal vector using the data from the normal map
	//Transform the normal map vector into local pixel space
	return normalize(mul(mapNormal, TBN));
}