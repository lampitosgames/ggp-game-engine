//LIGHT STRUCTS
struct DirectionalLight {
	float4 color;
	float3 direction;
	float diffuseIntensity;
	float ambientIntensity;
	float3 empty;
};

struct PointLight {
	float4 color;
	float3 position;
	float intensity;
	float range;
	float3 empty;
};

struct SpotLight {
	float4 color;
	float3 position;
	float intensity;
	float3 direction;
	float range;
	float cutoffAngle;
	float falloffRate;
	float2 empty;
};

//LIGHTING DATA REGISTERS
static const uint maxDirLightCount = 4;
cbuffer dirLightData : register(b1) {
	DirectionalLight dirLights[maxDirLightCount];
	matrix dirLightProj[maxDirLightCount];
	matrix dirLightView[maxDirLightCount];
	uint dirLightCount;
}

static const uint maxPointLightCount = 128;
cbuffer pointLightData : register(b2) {
	PointLight pointLights[maxPointLightCount];
	matrix pointLightProj[maxPointLightCount];
	matrix pointLightView[maxPointLightCount * 6];
	uint pointLightCount;
}

static const uint maxSpotLightCount = 128;
cbuffer spotLightData : register(b3) {
	SpotLight spotLights[maxSpotLightCount];
	matrix spotLightProj[maxSpotLightCount];
	matrix spotLightView[maxSpotLightCount];
	uint spotLightCount;
}