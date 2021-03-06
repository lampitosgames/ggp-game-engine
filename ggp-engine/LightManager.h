#ifndef GGP_LIGHT_MANAGER_H
#define GGP_LIGHT_MANAGER_H

#include <map>
#include <SimpleMath.h>
#include "PointLight.h"
#include "SpotLight.h"
#include "DirLight.h"
#include "LightStructs.h"
#include "SimpleShader.h"
#include "MeshRenderer.h"
class SystemManager;
class GameObject;

typedef UINT DirLightID;
typedef UINT PointLightID;
typedef UINT SpotLightID;

class LightManager {
	//Singleton pointer
	static LightManager* instance;
	//Device and context for creating shadow maps
	static ID3D11Device* dxDevice;
	static ID3D11DeviceContext* dxContext;

	//Unique ids given to each directional light
	DirLightID dlCount = 0;
	//Map of all directional lights
	std::map<DirLightID, DirLight*> dirLightUIDMap;
	//Directional light struct array of fixed length. Helps upload to the shader
	static const UINT maxDirLights = 4;
	DirLightStruct dirLights[maxDirLights];
	//Shadow vars for directional lights	
	ID3D11DepthStencilView* dirDepthStencilArr[maxDirLights];
	ID3D11ShaderResourceView* dirSRV; //Shader resource view

	//Unique ids given to each point light
	PointLightID plCount = 0;
	//Map of all point lights
	std::map<PointLightID, PointLight*> pointLightUIDMap;
	//Point light struct array of fixed length. Helps upload to the shader
	static const UINT maxPointLights = 32;
	PointLightStruct pointLights[maxPointLights];
	//Shadow vars for point lights
	ID3D11DepthStencilView* pointDepthStencilArr[maxPointLights * 6];
	ID3D11ShaderResourceView* pointSRV;

	//Unique ids given to each spot light
	SpotLightID slCount = 0;
	//Map of all spot lights
	std::map<SpotLightID, SpotLight*> spotLightUIDMap;
	//Spot light struct array of fixed length. Helps upload to the shader
	static const UINT maxSpotLights = 32;
	SpotLightStruct spotLights[maxSpotLights];

	//Shadow calculation globals
	static const UINT shadowMapSize = 2048; //Shadow map resolution
	ID3D11RasterizerState* shadowRast; //Rasterizer state for shadows
	SimpleVertexShader* shadowVS; //Specialized shadow vertex shader
	ID3D11SamplerState* samplerState; //Comparison sampler state

	SystemManager* systemManager;

public:
	//Static singleton get/release
	static LightManager* GetInstance();
	static void ReleaseInstance();

	// We don't want anything making copies of this class so delete these operators
	LightManager(LightManager const&) = delete;
	void operator=(LightManager const&) = delete;

	void Start();

	void RenderShadows(const std::map<UINT, MeshRenderer*> &_meshes);
	void UploadAllLights(SimplePixelShader* _pixelShader);
	void UploadAllShadows(SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader);

	UINT GetShadowMapResolution();
	ID3D11RasterizerState* GetShadowRasterizer();
	SimpleVertexShader* GetShadowVertexShader();
	ID3D11DepthStencilView* GetShadowDSV();
	ID3D11ShaderResourceView* GetShadowSRV();
	ID3D11SamplerState* GetShadowSampler();

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
	void InitDirShadowData();
	void InitPointShadowData();
	LightManager();
	~LightManager();
	void Release();
};

#endif //GGP_LIGHT_MANAGER_H