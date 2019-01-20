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
#include "PhysicsManager.h"
#include "ComponentManager.h"
#include "Scene.h"

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
	//Regen all render targets
	void RegenRenderTargets();

	//MOUSE VARIABLES
	//TODO: Move things like this to a system singleton
	//Is mouse cursor locked to the window?
	bool mouseLocked;
	POINT mousePos;
	POINT prevMousePos;

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ResourceManager* resourceManager;
	RenderManager* renderManager;
	InputManager* inputManager;
	LightManager* lightManager;
	ParticleManager* particleManager;
	Physics::PhysicsManager* physicsManager;
	ECS::ComponentManager* componentManager;

	//Active scene
	Scene* activeScene;
    GameObject* SelectedObj = nullptr;

	//Post Process - Bloom
	ID3D11RenderTargetView* ppRTV; //render to a texture <--- before pp
	ID3D11ShaderResourceView* ppSRV; //to sample from the same texture <--- original

	ID3D11RenderTargetView* extractRTV; //RRT for extracting bright pixels
	ID3D11ShaderResourceView* extractSRV; //texture with extracted pixels ready to blur

	ID3D11RenderTargetView* blurRTV; // RRT for Gaussian blur
	ID3D11ShaderResourceView* blurSRV;

	SimpleVertexShader* ppVS;
	SimplePixelShader* ppExtract;
	SimplePixelShader* ppBlur; //does 2 passes
	SimplePixelShader* addBlend; 

	ID3D11SamplerState* bloomSampler;

	//Post Proces - DoF
	ID3D11SamplerState* dofSampler;
	ID3D11RenderTargetView* RTarray[2];										
	ID3D11RenderTargetView* depthRTV;
	ID3D11ShaderResourceView* depthSRV;
	ID3D11RenderTargetView* dofBlurRTV;
	ID3D11ShaderResourceView* dofBlurSRV;
	ID3D11RenderTargetView* dofRTV;
	ID3D11ShaderResourceView* dofSRV;

	SimplePixelShader* dofPS; //post process

    int blurAmount = 15;
};

#endif //GGP_GAME_H
