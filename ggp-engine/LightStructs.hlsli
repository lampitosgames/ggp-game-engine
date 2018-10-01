
struct DirectionalLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
	float1 empty;
};

struct PointLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 position;
};