//LIGHT STRUCTS
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

//LIGHTING DATA REGISTERS
static const uint maxDirLightCount = 2;
cbuffer dirLightData : register(b1) {
	DirectionalLight dirLights[maxDirLightCount];
	uint dirLightCount;
}

static const uint maxPointLightCount = 6;
cbuffer pointLightData : register(b2) {
	PointLight pointLights[maxPointLightCount];
	uint pointLightCount;
}

