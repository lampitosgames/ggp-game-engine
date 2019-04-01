#include "stdafx.h"

#include <dxgidebug.h>
#include <WindowsX.h>
#include <sstream>
#include "SystemManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "Scene.h"

using namespace std;

SystemManager* SystemManager::instance = nullptr;

SystemManager* SystemManager::GetInstance() {
	if (instance == nullptr) {
		instance = new SystemManager();
	}
	return instance;
}

void SystemManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void SystemManager::CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns) {
	// Our temp console info struct
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// Get the console info and set the number of lines
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = bufferLines;
	coninfo.dwSize.X = bufferColumns;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = windowColumns;
	rect.Bottom = windowLines;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

	FILE *stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);

	// Prevent accidental console window close
	HWND consoleHandle = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
}

void SystemManager::UpdateTimer() {
	// Grab the current time
	__int64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	currentTime = now;

	// Calculate delta time and clamp to zero
	//  - Could go negative if CPU goes into power save mode 
	//    or the process itself gets moved to another core
	deltaTime = max((float)((currentTime - previousTime) * perfCounterSeconds), 0.0f);

	// Calculate the total time from start to now
	totalTime = (float)((currentTime - startTime) * perfCounterSeconds);

	// Save current time for next frame
	previousTime = currentTime;
}

void SystemManager::UpdateTitleBarStats() {
	fpsFrameCount++;

	// Only calc FPS and update title bar once per second
	float timeDiff = totalTime - fpsTimeElapsed;
	if (timeDiff < 1.0f)
		return;

	// How long did each frame take?  (Approx)
	float mspf = 1000.0f / (float)fpsFrameCount;

	// Quick and dirty title bar text (mostly for debugging)
	std::ostringstream output;
	output.precision(6);
	output << titleBarText <<
		"    Width: " << width <<
		"    Height: " << height <<
		"    FPS: " << fpsFrameCount <<
		"    Frame Time: " << mspf << "ms";

	// Append the version of DirectX the app is using
	switch (dxFeatureLevel) {
	case D3D_FEATURE_LEVEL_11_1: output << "    DX 11.1"; break;
	case D3D_FEATURE_LEVEL_11_0: output << "    DX 11.0"; break;
	case D3D_FEATURE_LEVEL_10_1: output << "    DX 10.1"; break;
	case D3D_FEATURE_LEVEL_10_0: output << "    DX 10.0"; break;
	case D3D_FEATURE_LEVEL_9_3:  output << "    DX 9.3";  break;
	case D3D_FEATURE_LEVEL_9_2:  output << "    DX 9.2";  break;
	case D3D_FEATURE_LEVEL_9_1:  output << "    DX 9.1";  break;
	default:                     output << "    DX ???";  break;
	}

	// Actually update the title bar and reset fps data
	SetWindowText(hWnd, output.str().c_str());
	fpsFrameCount = 0;
	fpsTimeElapsed += 1.0f;
}

void SystemManager::HandleMouseMove() {
	mouseLocked = inputManager->GetMouseLocked();
	//Store previous mouse position
	prevMousePos.x = mousePos.x;
	prevMousePos.y = mousePos.y;
	//Get the new mouse position
	GetCursorPos(&mousePos);
	if (mouseLocked) {
		//Store window rect
		RECT windowRect;
		GetWindowRect(hWnd, &windowRect);
		//Previous position is always the window's center when the cursor is locked
		prevMousePos.x = windowRect.left + width / 2;
		prevMousePos.y = windowRect.top + height / 2;
		//Reset cursor position
		SetCursorPos(prevMousePos.x, prevMousePos.y);
	}
	//Get mouse displacement
	int displaceX = mousePos.x - prevMousePos.x;
	int displaceY = mousePos.y - prevMousePos.y;
	//Pass data to input manager if mouse moved
	if (displaceX != 0 || displaceY != 0) {
		inputManager->_OnMouseMove(prevMousePos.x, prevMousePos.y, mousePos.x, mousePos.y);
	}
}

void SystemManager::OnMouseDown(WPARAM buttonState, int x, int y) {
	//Pass event to the input manager
	inputManager->_OnMouseDown(buttonState, x, y);
	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

void SystemManager::OnMouseUp(WPARAM buttonState, int x, int y) {
	//Pass event to the input manager
	inputManager->_OnMouseUp(buttonState, x, y);

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

void SystemManager::OnMouseMove(WPARAM buttonState, int x, int y) {
	//Currently does nothing
}

void SystemManager::OnMouseWheel(float wheelDelta, int x, int y) {
	//Pass event to the input manager
	inputManager->_OnMouseWheel(wheelDelta);
}

LRESULT SystemManager::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return instance->ProcessMessage(hWnd, uMsg, wParam, lParam);
}

#if defined(ENABLE_UI)
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
LRESULT SystemManager::ProcessMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#if defined(ENABLE_UI)
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
#endif

	// Check the incoming message and handle any we care about
	switch (uMsg) {
		// This is the message that signifies the window closing
	case WM_DESTROY:
		PostQuitMessage(0); // Send a quit message to our own program
		return 0;

		// Prevent beeping when we "alt-enter" into fullscreen
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

		// Prevent the overall window from becoming too small
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

		// Sent when the window size changes
	case WM_SIZE:
		// Don't adjust anything when minimizing,
		// since we end up with a width/height of zero
		// and that doesn't play well with DirectX
		if (wParam == SIZE_MINIMIZED)
			return 0;

		// Save the new client area dimensions.
		width = LOWORD(lParam);
		height = HIWORD(lParam);

		// If DX is initialized, resize 
		// our required buffers
		if (dxDevice)
			OnResize();

		return 0;

		// Mouse button being pressed (while the cursor is currently over our window)
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		// Mouse button being released (while the cursor is currently over our window)
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		// Cursor moves over the window (or outside, while we're currently capturing it)
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		// Mouse wheel is scrolled
	case WM_MOUSEWHEEL:
		OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	// Let Windows handle any messages we're not touching
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void SystemManager::Init(HINSTANCE hInstance, char * titleBarText, unsigned int windowWidth, unsigned int windowHeight, bool debugTitleBarStats) {
	// Save params
	this->hInstance = hInstance;
	this->titleBarText = titleBarText;
	this->width = windowWidth;
	this->height = windowHeight;
	this->titleBarStats = debugTitleBarStats;

	// Grab the start time now that the game is running
	__int64 now;
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	startTime = now;
	currentTime = now;
	previousTime = now;

	//Get singletons
	inputManager = InputManager::GetInstance();
	renderManager = RenderManager::GetInstance();
}

HRESULT SystemManager::InitWindow() {
	// Start window creation by filling out the
	// appropriate window class struct
	// Zero out the memory
	WNDCLASS wndClass = {};
	// Redraw on horizontal or vertical movement/adjustment
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = SystemManager::WindowProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	// Our app's handle
	wndClass.hInstance = hInstance;
	// Default icon
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// Default arrow cursor
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "Direct3DWindowClass";

	// Attempt to register the window class we've defined
	if (!RegisterClass(&wndClass)) {
		// Get the most recent error
		DWORD error = GetLastError();

		// If the class exists, that's actually fine.  Otherwise,
		// we can't proceed with the next step.
		if (error != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(error);
	}

	// Adjust the width and height so the "client size" matches
	// the width and height given (the inner-area of the window)
	RECT clientRect;
	SetRect(&clientRect, 0, 0, width, height);
	AdjustWindowRect(
		&clientRect,
		WS_OVERLAPPEDWINDOW,	// Has a title bar, border, min and max buttons, etc.
		false);					// No menu bar

	// Center the window to the screen
	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);
	int centeredX = (desktopRect.right / 2) - (clientRect.right / 2);
	int centeredY = (desktopRect.bottom / 2) - (clientRect.bottom / 2);

	// Actually ask Windows to create the window itself
	// using our settings so far.  This will return the
	// handle of the window, which we'll keep around for later
	hWnd = CreateWindow(
		wndClass.lpszClassName,
		titleBarText.c_str(),
		WS_OVERLAPPEDWINDOW,
		centeredX,
		centeredY,
		clientRect.right - clientRect.left,	// Calculated width
		clientRect.bottom - clientRect.top,	// Calculated height
		0,			// No parent window
		0,			// No menu
		hInstance,	// The app's handle
		0);			// No other windows in our application

	// Ensure the window was created properly
	if (hWnd == NULL) {
		DWORD error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif

	// The window exists but is not visible yet
	// We need to tell Windows to show it, and how to show it
	ShowWindow(hWnd, SW_SHOW);

	// Return an "everything is ok" HRESULT value
	return S_OK;
}

HRESULT SystemManager::InitDirectX() {
	// This will hold options for DirectX initialization
	unsigned int deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// If we're in debug mode in visual studio, we also
	// want to make a "Debug DirectX Device" to see some
	// errors and warnings in Visual Studio's output window
	// when things go wrong!
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create a description of how our swap
	// chain should work
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = 0;
	swapDesc.OutputWindow = hWnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = true;

	// Result variable for below function calls
	HRESULT hr = S_OK;

	// Attempt to initialize DirectX
	hr = D3D11CreateDeviceAndSwapChain(
		0,							// Video adapter (physical GPU) to use, or null for default
		D3D_DRIVER_TYPE_HARDWARE,	// We want to use the hardware (GPU)
		0,							// Used when doing software rendering
		deviceFlags,				// Any special options
		0,							// Optional array of possible verisons we want as fallbacks
		0,							// The number of fallbacks in the above param
		D3D11_SDK_VERSION,			// Current version of the SDK
		&swapDesc,					// Address of swap chain options
		&swapChain,					// Pointer to our Swap Chain pointer
		&dxDevice,					// Pointer to our Device pointer
		&dxFeatureLevel,			// This will hold the actual feature level the app will use
		&dxContext);					// Pointer to our Device Context pointer
	if (FAILED(hr)) return hr;

	// The above function created the back buffer render target
	// for us, but we need a reference to it
	ID3D11Texture2D* backBufferTexture;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);

	// Now that we have the texture, create a render target view
	// for the back buffer so we can render into it.  Then release
	// our local reference to the texture, since we have the view.
	dxDevice->CreateRenderTargetView(backBufferTexture,	0, &backBufferRTV);
	backBufferTexture->Release();

	// Set up the description of the texture to use for the depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	// Create the depth buffer and its view, then 
	// release our reference to the texture
	ID3D11Texture2D* depthBufferTexture;
	dxDevice->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	dxDevice->CreateDepthStencilView(depthBufferTexture, 0, &depthStencilView);
	depthBufferTexture->Release();

	// Bind the views to the pipeline, so rendering properly 
	// uses their underlying textures
	dxContext->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

	// Lastly, set up a viewport so we render into
	// to correct portion of the window
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dxContext->RSSetViewports(1, &viewport);

	// Enable IMGUI ----------------------------------------
#if defined(ENABLE_UI)

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO & io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(dxDevice, dxContext);
	ImGui::StyleColorsDark();

#endif

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	dxContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Return the "everything is ok" HRESULT value
	return S_OK;
}

void SystemManager::Update() {
	//Update timer and title bar
	UpdateTimer();
	if (titleBarStats)
		UpdateTitleBarStats();

	ID3D11DeviceContext* _dc = GetContext();

	//Handle mouse movement
	HandleMouseMove();
}

void SystemManager::Draw(float _deltaTime) {
	// Background color (Black in this case) for clearing
	const float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	dxContext->ClearRenderTargetView(backBufferRTV, color);

	dxContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//Call render on the renderManager
	renderManager->Render();

#if defined(ENABLE_UI)
	// Create a new IMGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Draw the UI options here -----------------------------------
	{
		ImGui::Begin("Info");
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);

		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Separator();

		ImGui::Text("Mouse   - Rotate");
		ImGui::Text("WASD    - Move");
		ImGui::Text("E       - Go Up");
		ImGui::Text("Q       - Go Down");
		ImGui::Text("X       - Release Mouse");

		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

HWND SystemManager::GetWindowHandle() { return this->hWnd; }

float SystemManager::GetDeltaTime() { return this->deltaTime; }

float SystemManager::GetTotalTime() { return this->totalTime; }

ID3D11Device * SystemManager::GetDevice() {
	return this->dxDevice;}

ID3D11DeviceContext * SystemManager::GetContext() { 
	return this->dxContext;
}

void SystemManager::OnResize() {
	// Release existing DirectX views and buffers
	if (depthStencilView) { depthStencilView->Release(); }
	if (backBufferRTV) { backBufferRTV->Release(); }

	// Resize the underlying swap chain buffers
	swapChain->ResizeBuffers(1,	width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// Recreate the render target view for the back buffer
	// texture, then release our local texture reference
	ID3D11Texture2D* backBufferTexture;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
	dxDevice->CreateRenderTargetView(backBufferTexture, 0, &backBufferRTV);
	backBufferTexture->Release();

	// Set up the description of the texture to use for the depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	// Create the depth buffer and its view, then 
	// release our reference to the texture
	ID3D11Texture2D* depthBufferTexture;
	dxDevice->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	dxDevice->CreateDepthStencilView(depthBufferTexture, 0, &depthStencilView);
	depthBufferTexture->Release();

	// Bind the views to the pipeline, so rendering properly 
	// uses their underlying textures
	dxContext->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);

	// Lastly, set up a viewport so we render into
	// to correct portion of the window
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dxContext->RSSetViewports(1, &viewport);

	//Pass resize event to scene
	Scene::activeScene->OnResize((float)width, (float)height);
}

void SystemManager::OnResize(ID3D11RenderTargetView * rtv) {
	// Release existing DirectX views and buffers
	if (depthStencilView) { depthStencilView->Release(); }
	if (rtv) { rtv->Release(); }

	// Resize the underlying swap chain buffers
	swapChain->ResizeBuffers(
		1,
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0);

	// Recreate the render target view for the back buffer
	// texture, then release our local texture reference
	ID3D11Texture2D* backBufferTexture;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferTexture));
	dxDevice->CreateRenderTargetView(backBufferTexture, 0, &rtv);
	backBufferTexture->Release();

	// Set up the description of the texture to use for the depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;

	// Create the depth buffer and its view, then 
	// release our reference to the texture
	ID3D11Texture2D* depthBufferTexture;
	dxDevice->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
	dxDevice->CreateDepthStencilView(depthBufferTexture, 0, &depthStencilView);
	depthBufferTexture->Release();

	// Bind the views to the pipeline, so rendering properly 
	// uses their underlying textures
	dxContext->OMSetRenderTargets(1, &rtv, depthStencilView);

	// Lastly, set up a viewport so we render into
	// to correct portion of the window
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dxContext->RSSetViewports(1, &viewport);
}

SystemManager::SystemManager() {
	// Initialize fields
	fpsFrameCount = 0;
	fpsTimeElapsed = 0.0f;
	// Query performance counter for accurate timing information
	__int64 perfFreq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&perfFreq);
	perfCounterSeconds = 1.0 / (double)perfFreq;
	//Init directX objects
	dxDevice = 0;
	dxContext = 0;
	swapChain = 0;
	backBufferRTV = 0;
	depthStencilView = 0;
	//Is the cursor locked (Default yes)
	mouseLocked = true;
	ShowCursor(true);
}

SystemManager::~SystemManager() {
	Release();
}

void SystemManager::Release() {
#ifdef ENABLE_UI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
	// Release all DirectX resources
	if (depthStencilView) { depthStencilView->Release(); }
	if (backBufferRTV) { backBufferRTV->Release(); }
	if (swapChain) { swapChain->Release(); }
	if (dxContext) { dxContext->Release(); }
	if (dxDevice) { dxDevice->Release(); }
}
