#pragma once

// Memory Leak detection ----------------
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <iostream>

// Common Libraries ---------------------
#include <DirectXMath.h>

/**************************************************************/
/* Platform specifications                                    */
/**************************************************************/
#if defined( _WIN32 ) || defined ( _WIN64 )

#include <DirectXMath.h>	// XMFLOAT3, XMFLOAT4X4
#include <d3d11.h>

using FileName = std::wstring;

#else 

// Using other another graphics library

using FileName = std::string;

#endif

// Debug Settings --------------------
#ifdef DEBUG

#define DEBUG_PRINT(a, ...) printf("%s: %d(): " a "\n", __FILE__, __LINE__, __VA_ARGS__)

#else

#define DEBUG_PRINT(a, ...)

#endif

// Typedefs -----------------------------

typedef unsigned int UINT;
