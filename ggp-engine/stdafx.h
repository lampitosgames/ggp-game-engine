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

// Debug Settings --------------------
#ifdef DEBUG

#define DEBUG_PRINT(a, ...) printf("%s: %d(): " a "\n", __FILE__, __LINE__, __VA_ARGS__)

#else

#define DEBUG_PRINT(a, ...)

#endif

// Typedefs -----------------------------

typedef unsigned int UINT;
