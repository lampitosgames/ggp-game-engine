#ifndef GGP_SYSTEM_MANAGER_H
#define GGP_SYSTEM_MANAGER_H

#define  ENABLE_UI
#if defined(ENABLE_UI)
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#endif

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>
class ResourceManager;
class RenderManager;
class InputManager;
class LightManager;
class ParticleManager;
class ComponentManager;

class SystemManager {
public:
	//Static Singleton get/release for the single input manager instance
	static SystemManager* GetInstance();
	static void ReleaseInstance();
	//Static os-level message processing things
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//Initialize variables and the running instance
	void Init(HINSTANCE hInstance, char* titleBarText, unsigned int windowWidth, unsigned int windowHeight, bool debugTitleBarStats);
	//Initialize the OS window
	HRESULT InitWindow();
	//Initialize directX
	HRESULT InitDirectX();
	//Debugging option for dumping directx objects still in use
	void ReportLiveObjects();

	//Update the singleton and timer data
	void Update();
	//Draw the game
	void Draw(float _deltaTime);

	/*
		Get/Set
	*/
	HWND GetWindowHandle();
	float GetDeltaTime();
	float GetTotalTime();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	D3D11_VIEWPORT GetViewport();
	ID3D11RenderTargetView* GetDefaultRTV();
	ID3D11DepthStencilView* GetDefaultDSV();
private:
	//Singleton pointer
	static SystemManager* instance;
	//Private constructor/destructor
	SystemManager();
	~SystemManager();
	void Release();
	// We don't want anything making copies of this class so delete these operators
	SystemManager(SystemManager const&) = delete;
	void operator=(SystemManager const&) = delete;

	//Resize handlers
	void OnResize();
	void OnResize(ID3D11RenderTargetView* rtv);

	//Helper function for allocating a console window
	void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);
	//Internal method for message handling
	LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//Singleton references that the system needs to update on changes
	ResourceManager* resourceManager;
	RenderManager* renderManager;
	InputManager* inputManager;
	LightManager* lightManager;
	ParticleManager* particleManager;
	ComponentManager* componentManager;

	//The handle to the application
	HINSTANCE hInstance;
	//The handle to the window itself
	HWND hWnd;
	//Custom text in window's title bar
	std::string titleBarText;
	//Show extra stats in title bar?
	bool titleBarStats;
	//Size of the window's client area
	unsigned int width;
	unsigned int height;
	//DirectX related objects and variables
	D3D11_VIEWPORT viewport;
	D3D_FEATURE_LEVEL dxFeatureLevel;
	IDXGISwapChain* swapChain;
	ID3D11Device* dxDevice;
	ID3D11DeviceContext* dxContext;
	Microsoft::WRL::ComPtr<ID3D11Debug> dxDebug;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;

	//Timing related data
	double perfCounterSeconds;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 previousTime;
	int fpsFrameCount;
	float fpsTimeElapsed;
	// Updates the timer for this frame
	void UpdateTimer();
	// Puts debug info in the title bar
	void UpdateTitleBarStats();

	//Mouse event handling
	bool mouseLocked;
	POINT mousePos;
	POINT prevMousePos;
	void HandleMouseMove();
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);
};

#endif //GGP_SYSTEM_MANAGER_H