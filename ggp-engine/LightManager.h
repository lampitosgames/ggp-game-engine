#ifndef GGP_LIGHT_MANAGER_H
#define GGP_LIGHT_MANAGER_H

#include <map>
#include <DirectXMath.h>
#include "PointLight.h"
#include "SpotLight.h"
#include "DirLight.h"
#include "LightStructs.h"
#include "SimpleShader.h"
class GameObject;

typedef UINT DirLightID;
typedef UINT PointLightID;
typedef UINT SpotLightID;

class LightManager {
	//Singleton pointer
	static LightManager* instance;

	//Unique ids given to each directional light
	DirLightID dlCount = 0;
	//Map of all directional lights
	std::map<DirLightID, DirLight*> dirLightUIDMap;
	//Directional light struct array of fixed length. Helsp upload to the shader
	const UINT maxDirLights = 4;
	DirLightStruct dirLights[4];

	//Unique ids given to each point light
	PointLightID plCount = 0;
	//Map of all point lights
	std::map<PointLightID, PointLight*> pointLightUIDMap;
	//Point light struct array of fixed length. Helps upload to the shader
	const UINT maxPointLights = 128;
	PointLightStruct pointLights[128];

	//Unique ids given to each spot light
	SpotLightID slCount = 0;
	//Map of all spot lights
	std::map<SpotLightID, SpotLight*> spotLightUIDMap;
	//Spot light struct array of fixed length. Helps upload to the shader
	const UINT maxSpotLights = 128;
	SpotLightStruct spotLights[128];
public:
	//Static singleton get/release
	static LightManager* GetInstance();
	static void ReleaseInstance();

	// We don't want anything making copies of this class so delete these operators
	LightManager(LightManager const&) = delete;
	void operator=(LightManager const&) = delete;

	void UploadAllLights(SimplePixelShader* _pixelShader);

	/*
		DIRECTIONAL LIGHT HELPERS
		Useful for managing directional light components
	*/
	DirLightID AddDirLight(DirLight* _dirLight);
	DirLight* GetDirLight(DirLightID _uniqueID);
	void RemoveDirLight(DirLight* _dirLight);
	DirLightStruct GetDirLightStruct(DirLightID _uniqueID);

	/*
		POINT LIGHT HELPERS
		Used for adding, getting, and deleting point light components
	*/
	PointLightID AddPointLight(PointLight* _pointLight);
	PointLight* GetPointLight(PointLightID _uniqueID);
	void RemovePointLight(PointLight* _pointLight);
	PointLightStruct GetPointLightStruct(PointLightID _uniqueID);

	/*
	SPOT LIGHT HELPERS
	Used for adding, getting, and deleting spot light components
	*/
	SpotLightID AddSpotLight(SpotLight* _spotLight);
	SpotLight* GetSpotLight(SpotLightID _uniqueID);
	void RemoveSpotLight(SpotLight* _spotLight);
	SpotLightStruct GetSpotLightStruct(SpotLightID _uniqueID);
private:
	LightManager();
	~LightManager();
	void Release();
};

#endif //GGP_LIGHT_MANAGER_H