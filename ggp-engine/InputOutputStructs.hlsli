// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput {
	float3 position		: POSITION;     // XYZ position
	float2 uv           : TEXCOORD;     // UV Coordinates for main texture
	float3 normal       : NORMAL;       // Normal vector
	float3 tangent      : TANGENT;      // Tangent vector
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel {
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;       // Normal vector
	float2 uv           : TEXCOORD;     // UV Coordinates
	float3 tangent      : TANGENT;      // Tangent vector
	float3 worldPos     : POSITION;     // World-space position of the vertex
};


// Struct representing the particle shader input
struct ParticleVertexShaderInput {
	//Per-vertex
	float2 uv           : TEXCOORD0;
	//Per-instance
	float3 iPos         : INITIAL_POSITION;
	float3 iVel         : INITIAL_VELOCITY;
	float3 accel        : ACCELERATION;
	float4 color        : TEXCOORD1;
	float startLife     : INITIAL_TIME;
	float remainLife    : REMAIN_TIME;
};

struct ParticleVertexToPixel {
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
	float4 color		: TEXCOORD1;
};