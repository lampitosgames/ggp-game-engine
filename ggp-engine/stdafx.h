#pragma once

// Memory Leak detection ----------------
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <locale> 
#include <codecvt>

// Common Libraries ---------------------
#include <d3d11.h>

// Debug Settings --------------------
#ifdef DEBUG

#define DEBUG_PRINT(a, ...) printf("%s: %d(): " a "\n", __FILE__, __LINE__, __VA_ARGS__)

#else

#define DEBUG_PRINT(a, ...)

#endif

// Typedefs -----------------------------
static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> sconvert;

typedef unsigned int UINT;
typedef std::string ResName;