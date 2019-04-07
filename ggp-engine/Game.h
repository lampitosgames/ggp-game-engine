#ifndef GGP_GAME_H
#define GGP_GAME_H

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "ResourceManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "LightManager.h"
#include "ParticleManager.h"
#include "ComponentManager.h"
#include "Scene.h"

class Game : public DXCore {
public:
	Game();
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void Update(float deltaTime, float totalTime);
private:
	//Singleton instance pointers
	ResourceManager* resourceManager;
	RenderManager* renderManager;
	InputManager* inputManager;
	LightManager* lightManager;
	ParticleManager* particleManager;
	ComponentManager* componentManager;
};

#endif //GGP_GAME_H
