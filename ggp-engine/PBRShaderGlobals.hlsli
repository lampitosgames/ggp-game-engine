//Global definitions for cbuffers used in physically based rendering shaders.
//EXTERNAL DATA
cbuffer externalData : register(b0) {
	//Camera's position
	float3 cameraPosition;
	//Base material properties
	float4 baseColor;
	float baseRoughness;
	float baseMetalness;
	//Gamma correction value
	float gammaModifier;
	//Boolean for using each texture channel
	int useAlbedoTex;
	int useNormalTex;
	int useRoughnessTex;
	int useMetalnessTex;
}