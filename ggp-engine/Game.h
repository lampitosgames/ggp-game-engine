#ifndef GGP_GAME_H
#define GGP_GAME_H

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "LightManager.h"
#include "Spatial.h"
#include "FlyingCamera.h"

class Game
	: public DXCore {

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	//Mouse move
	void HandleMouseMove();

	// Overridden mouse input helper methods
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);
private:
	//MOUSE VARIABLES
	//TODO: Move things like this to a system singleton
	//Is mouse cursor locked to the window?
	bool mouseLocked;
	POINT mousePos;
	POINT prevMousePos;

	// Initialization helper methods - feel free to customize, combine, etc.
	void CreateBasicGeometry();

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ResourceManager* resourceManager;
	RenderManager* renderManager;
	InputManager* inputManager;
	LightManager* lightManager;

	//Temporary game object storage until I implement scenes
	Spatial* gameObject1;
	Spatial* gameObject2;
	Spatial* gameObject3;
	Spatial* gameObject4;
	Spatial* gameObject5;

	Spatial* lightObject;
	Spatial* lightObject2;
	Spatial* lightObject3;

	FlyingCamera* activeCamera;
};

#endif //GGP_GAME_H
