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
	//Loop through and get every directional light
	std::map<UINT, DirLight*>::iterator dlIt;
	for (dlIt = dirLightUIDMap.begin(); dlIt != dirLightUIDMap.end(); ++dlIt) {
		//Add the directional light to the array
		dirLights[dirLightCount] = dlIt->second->buildLightStruct();
		dirLightCount += 1;
		//Break out of loop if we've hit the max number of lights
		if (dirLightCount >= maxDirLights) { break; }
	}
	_pixelShader->SetData("dirLights", &dirLights, sizeof(DirLightStruct) * maxDirLights);
	_pixelShader->SetData("dirLightCount", &dirLightCount, sizeof(UINT));

	//Loop through and get every point light
	std::map<UINT, PointLight*>::iterator plIt;
	for (plIt = pointLightUIDMap.begin(); plIt != pointLightUIDMap.end(); ++plIt) {
		pointLights[pointLightCount] = plIt->second->buildLightStruct();
		pointLightCount += 1;
		if (pointLightCount >= maxPointLights) { break; }
	}
	_pixelShader->SetData("pointLights", &pointLights, sizeof(PointLightStruct) * maxPointLights);
	_pixelShader->SetData("pointLightCount", &pointLightCount, sizeof(UINT));
}

#pragma region Directional Lights
UINT LightManager::AddDirLight(GameObject* _gameObject, XMFLOAT4 _color, XMFLOAT3 _direction, float _diffuseIntensity, float _ambientIntensity) {
	DirLight* tempDL = new DirLight(dlUID, _gameObject, _color, _direction, _diffuseIntensity, _ambientIntensity);;
	dirLightUIDMap[dlUID] = tempDL;
	dlUID++;
	return dlUID - 1;
}

DirLight* LightManager::GetDirLight(UINT _uniqueID) {
	auto thisDL = dirLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisDL != dirLightUIDMap.end()) {
		return thisDL->second;
	}
	return nullptr;
}

DirLightStruct LightManager::GetDirLightStruct(UINT _uniqueID) {
	DirLight* tempDL = GetDirLight(_uniqueID);
	if (tempDL != nullptr) {
		return tempDL->buildLightStruct();
	}
	return {};
}

void LightManager::DeleteDirLight(UINT _uniqueID) {
	DirLight* dlTemp = GetDirLight(_uniqueID);
	if (dlTemp) {
		delete dlTemp;
		dirLightUIDMap.erase(_uniqueID);
	}
}
#pragma endregion

#pragma region Point Lights
UINT LightManager::AddPointLight(Spatial* _gameObject) {
	return AddPointLight(_gameObject, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
}

UINT LightManager::AddPointLight(Spatial* _gameObject, XMFLOAT4 _color) {
	PointLight* tempPL = new PointLight(plUID, _gameObject, _color);
	pointLightUIDMap[plUID] = tempPL;
	plUID++;
	return plUID - 1;
}

PointLight* LightManager::GetPointLight(UINT _uniqueID) {
	auto thisPL = pointLightUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisPL != pointLightUIDMap.end()) {
		return thisPL->second;
	}
	return nullptr;
}

PointLightStruct LightManager::GetPointLightStruct(UINT _uniqueID) {
	PointLight* tempPL = GetPointLight(_uniqueID);
	if (tempPL != nullptr) {
		return tempPL->buildLightStruct();
	}
	return {};
}

void LightManager::DeletePointLight(UINT _uniqueID) {
	PointLight* plTemp = GetPointLight(_uniqueID);
	if (plTemp) {
		delete plTemp;
		pointLightUIDMap.erase(_uniqueID);
	}
}
#pragma endregion

LightManager::LightManager() {
}

LightManager::~LightManager() {
	Release();
}

void LightManager::Release() {
	//Loop through and delete every point light
	std::map<UINT, PointLight*>::iterator plIterator;
	for (plIterator = pointLightUIDMap.begin(); plIterator != pointLightUIDMap.end(); ++plIterator) {
		PointLight* plTemp = plIterator->second;
		if (plTemp != nullptr) {
			delete plTemp;
		}
	}
	//Reset point light unique ID values
	plUID = 0;
	//Clear the map so the singleton can be reused.
	pointLightUIDMap.clear();
	
	//Loop through and delete every directional light
	std::map<UINT, DirLight*>::iterator dlIterator;
	for (dlIterator = dirLightUIDMap.begin(); dlIterator != dirLightUIDMap.end(); ++dlIterator) {
		DirLight* dlTemp = dlIterator->second;
		if (dlTemp != nullptr) {
			delete dlTemp;
		}
	}
	//Reset directional light unique ID values
	dlUID = 0;
	//Clear the map so the singleton can be reused.
	dirLightUIDMap.clear();
}
