#include "stdafx.h"

#include "DXCore.h"
#include "SystemManager.h"

// Define the static instance variable so our OS-level 
// message handling function below can talk to our object
DXCore* DXCore::DXCoreInstance = 0;

DXCore::DXCore() {
	// Save a static reference to this object.
	//  - Since the OS-level message function must be a non-member (global) function, 
	//    it won't be able to directly interact with our DXCore object otherwise.
	//  - (Yes, a singleton might be a safer choice here).
	DXCoreInstance = this;
}

// --------------------------------------------------------
// Destructor - Clean up (release) all DirectX references
// --------------------------------------------------------
DXCore::~DXCore() {}

// --------------------------------------------------------
// This is the main game loop, handling the following:
//  - OS-level messages coming in from Windows itself
//  - Calling update & draw back and forth, forever
// --------------------------------------------------------
HRESULT DXCore::Run() {
	systemManager = SystemManager::GetInstance();
	// Give subclass a chance to initialize
	Init();
	// Our overall game and message loop
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		// Determine if there is a message waiting
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// Translate and dispatch the message
			// to our custom WindowProc function
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			//Update system singleton
			systemManager->Update();
			// The game loop
			Update(systemManager->GetDeltaTime(), systemManager->GetTotalTime());
		}
	}

	// We'll end up here once we get a WM_QUIT message,
	// which usually comes from the user closing the window
	return (HRESULT)msg.wParam;
}


// --------------------------------------------------------
// Sends an OS-level window close message to our process, which
// will be handled by our message processing function
// --------------------------------------------------------
void DXCore::Quit() {
	PostMessage(systemManager->GetWindowHandle(), WM_CLOSE, NULL, NULL);
}