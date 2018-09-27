
struct DirectionalLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
};

struct PointLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 position;
};