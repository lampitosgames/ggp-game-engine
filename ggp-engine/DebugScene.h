#ifndef GAME_DEBUG_SCENE_H
#define GAME_DEBUG_SCENE_H

#include "Scene.h"

class DebugScene : public Scene {
public:
	DebugScene(std::string _uniqueID) : Scene(_uniqueID) {}
	void Init();
	void Start();
	void Update(float _deltaTime);
};

#endif //GAME_DEBUG_SCENE_H