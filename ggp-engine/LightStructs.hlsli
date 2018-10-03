
struct DirectionalLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
	float empty;
};

struct PointLight {
	float4 color;
	float3 position;
	float ambientColor;
	float diffuseColor;
	float constAtten;
	float linearAtten;
	float expAtten;
};