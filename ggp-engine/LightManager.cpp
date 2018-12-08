#include "stdafx.h"

#include "LightManager.h"

using namespace DirectX;

LightManager* LightManager::instance = nullptr;

LightManager* LightManager::GetInstance() {
	if (instance == nullptr) {
		instance = new LightManager();
	}
	return instance;
}

void LightManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void LightManager::UploadAllLights(SimplePixelShader* _pixelShader) {
	//TODO: Only upload lights close to objects
	UINT dirLightCount = 0;
	UINT pointLightCount = 0;
	UINT spotLightCount = 0;
	//Loop through and get every directional light
	std::map<UINT, DirLight*>::iterator dlIt;
	for (dlIt = dirLightUIDMap.begin(); dlIt != dirLightUIDMap.end();) {
		if (dlIt->second == nullptr) {
			dirLightUIDMap.erase(dlIt++);
			continue;
		}
		//Add the directional light to the array
		dirLights[dirLightCount] = dlIt->second->buildLightStruct();
		dirLightCount += 1;
		//Break out of loop if we've hit the max number of lights
		if (dirLightCount >= maxDirLights) { break; }
		dlIt++;
	}
	_pixelShader->SetData("dirLights", &dirLights, sizeof(DirLightStruct) * maxDirLights);
	_pixelShader->SetData("dirLightCount", &dirLightCount, sizeof(UINT));

	//Loop through and get every point light
	std::map<UINT, PointLight*>::iterator plIt;
	for (plIt = pointLightUIDMap.begin(); plIt != pointLightUIDMap.end();) {
		if (plIt->second == nullptr) {
			pointLightUIDMap.erase(plIt++);
			continue;
		}
		pointLights[pointLightCount] = plIt->second->buildLightStruct();
		pointLightCount += 1;
		if (pointLightCount >= maxPointLights) { break; }
		plIt++;
	}
	_pixelShader->SetData("pointLights", &pointLights, sizeof(PointLightStruct) * maxPointLights);
	_pixelShader->SetData("pointLightCount", &pointLightCount, sizeof(UINT));

	//Loop through and get every spot light
	std::map<UINT, SpotLight*>::iterator slIt;
	for (slIt = spotLightUIDMap.begin(); slIt != spotLightUIDMap.end();) {
		if (slIt->second == nullptr) {
			spotLightUIDMap.erase(slIt++);
			continue;
		}
		spotLights[spotLightCount] = slIt->second->buildLightStruct();
		spotLightCount += 1;
		if (spotLightCount >= maxSpotLights) { break; }
		slIt++;
	}
	_pixelShader->SetData("spotLights", &spotLights, sizeof(SpotLightStruct) * maxSpotLights);
	_pixelShader->SetData("spotLightCount", &spotLightCount, sizeof(UINT));
}

#pragma region Directional Lights
DirLightID LightManager::AddDirLight(DirLight* _dirLight) {
	dirLightUIDMap[dlCount] = _dirLight;
	dlCount++;
	return dlCount - 1;
}

DirLight* LightManager::GetDirLight(DirLightID _uniqueID) {
	auto thisDL = dirLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisDL != dirLightUIDMap.end()) {
		return thisDL->second;
	}
	return nullptr;
}

void LightManager::RemoveDirLight(DirLight * _dirLight) {
	auto dlIt = dirLightUIDMap.begin();
	for (; dlIt != dirLightUIDMap.end(); ++dlIt) {
		if (dlIt->second == _dirLight) {
			dirLightUIDMap[dlIt->first] = nullptr;
		}
	}
}

DirLightStruct LightManager::GetDirLightStruct(DirLightID _uniqueID) {
	DirLight* tempDL = GetDirLight(_uniqueID);
	if (tempDL != nullptr) {
		return tempDL->buildLightStruct();
	}
	return {};
}
#pragma endregion

#pragma region Point Lights

PointLightID LightManager::AddPointLight(PointLight* _pointLight) {
	pointLightUIDMap[plCount] = _pointLight;
	plCount++;
	return plCount - 1;
}

PointLight* LightManager::GetPointLight(PointLightID _uniqueID) {
	auto thisPL = pointLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisPL != pointLightUIDMap.end()) {
		return thisPL->second;
	}
	return nullptr;
}

void LightManager::RemovePointLight(PointLight * _pointLight) {
	auto plIt = pointLightUIDMap.begin();
	for (; plIt != pointLightUIDMap.end(); ++plIt) {
		if (plIt->second == _pointLight) {
			pointLightUIDMap[plIt->first] = nullptr;
		}
	}
}

PointLightStruct LightManager::GetPointLightStruct(PointLightID _uniqueID) {
	PointLight* tempPL = GetPointLight(_uniqueID);
	if (tempPL != nullptr) {
		return tempPL->buildLightStruct();
	}
	return {};
}
#pragma endregion

#pragma region Spot Lights
SpotLightID LightManager::AddSpotLight(SpotLight* _spotLight) {
	spotLightUIDMap[slCount] = _spotLight;
	slCount++;
	return slCount - 1;
}

void LightManager::RemoveSpotLight(SpotLight * _spotLight) {
	auto slIt = spotLightUIDMap.begin();
	for (; slIt != spotLightUIDMap.end(); ++slIt) {
		if (slIt->second == _spotLight) {
			spotLightUIDMap[slIt->first] = nullptr;
		}
	}
}

SpotLight* LightManager::GetSpotLight(PointLightID _uniqueID) {
	auto thisSL = spotLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisSL != spotLightUIDMap.end()) {
		return thisSL->second;
	}
	return nullptr;
}

SpotLightStruct LightManager::GetSpotLightStruct(PointLightID _uniqueID) {
	SpotLight* tempSpotL = GetSpotLight(_uniqueID);
	if (tempSpotL != nullptr) {
		return tempSpotL->buildLightStruct();
	}
	return {};
}
#pragma endregion

LightManager::LightManager() {}

LightManager::~LightManager() {
	Release();
}

void LightManager::Release() {
	//Reset directional light unique ID values
	dlCount = 0;
	//Clear the map so the singleton can be reused.
	dirLightUIDMap.clear();
	//Reset point light unique ID values
	plCount = 0;
	//Clear the map so the singleton can be reused.
	pointLightUIDMap.clear();
	//Reset spot light unique ID values
	slCount = 0;
	//Clear the map so the singleton can be reused.
	spotLightUIDMap.clear();
}
