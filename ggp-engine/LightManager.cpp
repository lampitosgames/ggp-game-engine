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
}
