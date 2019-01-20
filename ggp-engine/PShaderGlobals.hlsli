//Global definitions for cbuffers used in pixel shaders. Helpful for standardizing pshader inputs.
//EXTERNAL DATA
cbuffer externalData : register(b0) {
	//Camera's position
	float3 cameraPosition;
	//Material properties
	float baseSpec;
	float4 baseColor;
	//Gamma correction value
	float gammaModifier;
	//Boolean for using each texture channel
	int useDiffuseTex;
	int useNormalTex;
	int useSpecularTex;
}