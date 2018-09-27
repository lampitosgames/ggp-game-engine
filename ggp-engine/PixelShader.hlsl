
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel {
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;       // Normal vector
};

struct DirectionalLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
};

cbuffer lightData : register(b0) {
	DirectionalLight light1;
	DirectionalLight light2;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET {
	//Normalize incoming normal vector
	input.normal = normalize(input.normal);

    float3 toLight1 = normalize(-light1.direction);
	float3 toLight2 = normalize(-light2.direction);

	float light1Amount = saturate(dot(toLight1, input.normal));
	float light2Amount = saturate(dot(toLight2, input.normal));

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return ((light1Amount * light1.diffuseColor) + light1.ambientColor) + ((light2Amount * light2.diffuseColor) + light2.ambientColor);
}