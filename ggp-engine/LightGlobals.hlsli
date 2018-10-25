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
	float constAtten;
	float linearAtten;
	float expAtten;
	float empty;
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

/*
	BASIC LIGHTING
*/
////HELPERS
//Diffuse calculation
float diffuse(float3 _normal, float3 _lightDir) {
	//Return N*L
	return saturate(dot(_normal, normalize(-_lightDir)));
}

//Specular calculation (full phong)
float specular(float3 _normal, float3 _lightDir, float3 _camPos, float3 _worldPos, float _shininess = 0.0f) {
	//Reflect the light direction across the normal to the surface
	float3 refl = reflect(normalize(_lightDir), _normal);
	//Get a normalized direction vector pointing at the camera
	float3 camDir = normalize(_camPos - _worldPos);
	//If there is any specularity on this surface, calculate it using phong
	float spec = _shininess == 0.0f ? 0.0f : _shininess * pow(saturate(dot(refl, camDir)), 64);
	//Return specularity scalar
	return spec;
}

////CALC FOR INDIVIDUAL TYPES
//Directional light
float3 calcDirLight(DirectionalLight _light, float3 _normal, float3 _surfaceColor, float3 _camPos, float3 _worldPos, float _shininess = 0.0f) {
	//Calculate light amounts
	float diff = diffuse(_normal, _light.direction);
	float spec = specular(_normal, _light.direction, _camPos, _worldPos, _shininess);
	//Combine
	return (diff * _surfaceColor + spec) * _light.color * _light.diffuseIntensity + _surfaceColor * _light.color * _light.ambientIntensity;
}

//Point Light
float3 calcPointLight(PointLight _light, float3 _normal, float3 _surfaceColor, float3 _camPos, float3 _worldPos, float _shininess = 0.0f) {
	//Get direction from the pixel to the point light
	float3 lightDir = _worldPos - _light.position;
	//Get the distance to the light
	float4 lightDist = length(lightDir);
	//After getting distance, normalize the dir vector
	lightDir = normalize(lightDir);
	//Attenuation calculation
	float lightAtten = 1.0f / (_light.constAtten + _light.linearAtten*lightDist + _light.expAtten*lightDist*lightDist);

	//Calculate lighting 
	float diff = diffuse(_normal, lightDir);
	float spec = specular(_normal, lightDir, _camPos, _worldPos, _shininess);

	//Return the combined lighting
	return (diff * _surfaceColor + spec) * lightAtten * _light.intensity * _light.color;
}


/*
	UTILS
*/
//Apply the normal from a texture to the normal and tangent vectors from the mesh
float3 applyNormalMap(float3 _normal, float3 _tangent, float3 _mapNormal) {
	//Construct TBN matrix
	//NOTE: I followed the demo on mycourses for this (Game Graphics Programming)
	//Normal
	float3 N = _normal;
	//Tangent
	float3 T = normalize(_tangent - N * dot(_tangent, N));
	//Bi-tangent
	float3 B = cross(T, N);
	//TBN Matrix
	float3x3 TBN = float3x3(T, B, N);
	//Adjust normal vector using the data from the normal map
	//Transform the normal map vector into local pixel space
	return normalize(mul(_mapNormal, TBN));
}
