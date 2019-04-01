#ifndef GGP_DXCORE_H
#define GGP_DXCORE_H


#include <Windows.h>
#include <d3d11.h>
#include <string>
class SystemManager;

// We can include the correct library files here
// instead of in Visual Studio settings if we want
#pragma comment(lib, "d3d11.lib")

class DXCore {
public:
	DXCore();
	~DXCore();

	// Static requirements for OS-level message processing
	static DXCore* DXCoreInstance;

	SystemManager* systemManager;

	HRESULT Run();
	void Quit();

	// Pure virtual methods for setup and game functionality
	virtual void Init() = 0;
	virtual void Update(float deltaTime, float totalTime) = 0;
};

#endif //GGP_DXCORE_H
