#include "stdafx.h"
#include "ComponentManager.h"
#include "GameObject.h"

// Singleton requirement
ComponentManager* ComponentManager::instance = nullptr;

ComponentManager* ComponentManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ComponentManager();
	}
	return instance;
}

void ComponentManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void ComponentManager::CleanupComponents(const EntityID _entityID) {
	if (activeComponents.find(_entityID) == activeComponents.end()) { return; }
	auto vec_itr = activeComponents[_entityID].begin();
	for (; vec_itr != activeComponents[_entityID].end(); ++vec_itr) {
		if (vec_itr->second != nullptr) {
			delete vec_itr->second;
			activeComponents[_entityID][vec_itr->first] = nullptr;
		}
	}
}

ComponentManager::ComponentManager() {}

ComponentManager::~ComponentManager() {
	CleanupAllComponents();
}

void ComponentManager::CleanupAllComponents() {
	auto map_itr = activeComponents.begin();
	for (; map_itr != activeComponents.end(); ++map_itr) {
		auto vec_itr = map_itr->second.begin();
		for (; vec_itr != map_itr->second.end(); ++vec_itr) {
			if ((vec_itr->second) != nullptr) {
				delete vec_itr->second;
			}
		}
	}
	ComponentCount = 0;
}