#ifndef GAME_DEBUG_SCENE_H
#define GAME_DEBUG_SCENE_H

#include "Scene.h"

class DebugScene : public Scene {
public:
	DebugScene(std::string _uniqueID) : Scene(_uniqueID) {}
	void Init();
	void Start();
	void Update(float _deltaTime);

    const float Gravity = -0.001f;

};

#endif //GAME_DEBUG_SCENE_H