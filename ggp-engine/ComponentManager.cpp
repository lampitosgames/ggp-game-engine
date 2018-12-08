#include "stdafx.h"
#include "ComponentManager.h"
#include "GameObject.h"

// Singleton requirement
ECS::ComponentManager* ECS::ComponentManager::instance = nullptr;

using namespace ECS;

ECS::ComponentManager* ECS::ComponentManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ComponentManager();
	}
	return instance;
}

void ECS::ComponentManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void ECS::ComponentManager::CleanupComponents(const EntityID aEntityID) {
	if (activeComponents.find(aEntityID) == activeComponents.end()) { return; }
	auto vec_itr = activeComponents[aEntityID].begin();
	for (; vec_itr != activeComponents[aEntityID].end(); ++vec_itr) {
		if (vec_itr->second != nullptr) {
			delete vec_itr->second;
			activeComponents[aEntityID][vec_itr->first] = nullptr;
		}
	}
}

ECS::ComponentManager::ComponentManager() {}

ECS::ComponentManager::~ComponentManager() {
	CleanupAllComponents();
}

void ECS::ComponentManager::CleanupAllComponents() {
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