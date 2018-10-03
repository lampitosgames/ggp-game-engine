#ifndef GGP_LIGHT_MANAGER_H
#define GGP_LIGHT_MANAGER_H

#include <map>
#include <DirectXMath.h>
#include "PointLight.h"
#include "DirLight.h"
#include "SimpleShader.h"

class LightManager {
	//Singleton pointer
	static LightManager* instance;

	//Unique ids given to each directional light
	UINT dlUID = 0;
	//Map of all directional lights
	std::map<UINT, DirLight*> directionLightUIDMap;

	//Unique ids given to each point light
	UINT plUID = 0;
	//Map of all point lights
	std::map<UINT, PointLight*> pointLightUIDMap;
	//Point light struct array of fixed length. Helps upload to the shader
	PointLightStruct pointLights[9];
public:
	//Static singleton get/release
	static LightManager* GetInstance();
	static void ReleaseInstance();

	/*
		DIRECTIONAL LIGHT HELPERS
		Useful for managing directional light components
	*/
	UINT AddDirLight(Spatial* _gameObject, 
							 DirectX::XMFLOAT4 _ambientColor = DirectX::XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f), 
							 DirectX::XMFLOAT4 _diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 
							 DirectX::XMFLOAT3 _direction = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	DirLight* GetDirLight(UINT _uniqueID);
	DirectionalLightStruct GetDirLightStruct(UINT _uniqueID);

	/*
		POINT LIGHT HELPERS
		Used for adding, getting, and deleting point light components
	*/
	//Create (and return the UID of) a new point light
	UINT AddPointLight(Spatial* _gameObject);
	UINT AddPointLight(Spatial* _gameObject, DirectX::XMFLOAT4 _color);
	//Get a point light given its unique ID
	PointLight* GetPointLight(UINT _uniqueID);
	//Get a struct representing the current state of the point light
	PointLightStruct GetPointLightStruct(UINT _uniqueID);
	//Delete the point light
	void DeletePointLight(UINT _uniqueID);
private:
	LightManager();
	~LightManager();
	void Release();
};

#endif //GGP_LIGHT_MANAGER_H