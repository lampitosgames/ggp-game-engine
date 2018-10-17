//Global definitions for cbuffers used in pixel shaders. Helpful for standardizing pshader inputs.
//EXTERNAL DATA
cbuffer externalData : register(b0) {
	//Camera's position
	float3 cameraPosition;
	//Material properties
	float baseSpec;
	float4 baseColor;
	//Boolean for using each texture channel
	// x - Diffuse texture
	// y - Normal map texture
	// z - Specular map texture
	int3 texChanToggle;
}