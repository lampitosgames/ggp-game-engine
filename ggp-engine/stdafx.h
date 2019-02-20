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

//Convert a std::string to a LPCWSTR
static std::wstring toWSTR(std::string _str) {
	int len;
	int slength = (int)_str.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, _str.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, _str.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

// Typedefs -----------------------------

typedef unsigned int UINT;
typedef std::string FileName;