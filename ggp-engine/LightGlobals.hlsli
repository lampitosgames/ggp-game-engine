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

struct SpotLight {
	float4 color;
	float3 direction;
	float cone;
	float3 position;
	float range;
};

//LIGHTING DATA REGISTERS
static const uint maxDirLightCount = 4;
cbuffer dirLightData : register(b1) {
	DirectionalLight dirLights[maxDirLightCount];
	uint dirLightCount;
}

static const uint maxPointLightCount = 128;
cbuffer pointLightData : register(b2) {
	PointLight pointLights[maxPointLightCount];
	uint pointLightCount;
}

static const uint maxSpotLightCount = 128;
cbuffer spotLightData : register(b2) {
	SpotLight spotLights[maxSpotLightCount];
	uint spotLightCount;
}
