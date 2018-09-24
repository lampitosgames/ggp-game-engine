#ifndef GGP_GAME_H
#define GGP_GAME_H

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "ResourceManager.h"
#include "MeshManager.h"
#include "InputManager.h"
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

	// Overridden mouse input helper methods
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void CreateBasicGeometry();

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ResourceManager* resourceManager;
	MeshManager* meshManager;
	InputManager* inputManager;

	//Temporary game object storage until I implement scenes
	Spatial* gameObject1;
	Spatial* gameObject2;
	Spatial* gameObject3;
	Spatial* gameObject4;
	Spatial* gameObject5;

	FlyingCamera* activeCamera;
};

#endif //GGP_GAME_H
