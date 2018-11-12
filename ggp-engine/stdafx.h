#pragma once

// Memory Leak detection ----------------
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <stdio.h>
#include <crtdbg.h>  
#include <assert.h>
#include <string>
#include <iostream>

// Replace the new keyword with the debug new 
#ifdef _DEBUG

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#endif

// Debug Settings --------------------
#ifdef _DEBUG

#define DEBUG_PRINT(a, ...) printf("%s: %d(): " a "\n", __FILE__, __LINE__, __VA_ARGS__);

#else

#define DEBUG_PRINT(a, ...)

#endif

// Typedefs -----------------------------

// TODO: UINT typedef

// Common Libraries ---------------------
#include <DirectXMath.h>