#ifndef GAME_PBR_DEMO_SCENE_H
#define GAME_PBR_DEMO_SCENE_H

#include "Scene.h"

class PBRDemoScene : public Scene {
public:
	PBRDemoScene(std::string _uniqueID) : Scene(_uniqueID) {}
	void Init();
	void Start();
	void Update(float _deltaTime);
};

#endif //GAME_PBR_DEMO_SCENE_H