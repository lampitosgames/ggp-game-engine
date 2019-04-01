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
#include <SimpleMath.h>

// Static util functions
namespace Util {
	static DirectX::SimpleMath::Vector3 GetEulerFromDir(DirectX::SimpleMath::Vector3 _dir) {
		_dir.Normalize();
		DirectX::SimpleMath::Vector3 zRotAxis = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
		DirectX::SimpleMath::Vector3 yRotAxis = _dir * -1.0f;
		DirectX::SimpleMath::Vector3 xRotAxis = yRotAxis.Cross(zRotAxis);

		DirectX::SimpleMath::Matrix rotMat = DirectX::SimpleMath::Matrix(xRotAxis, yRotAxis, zRotAxis);

		//TODO: Remove all of this when we redo transforms.
		//https://gamedev.stackexchange.com/questions/50963/how-to-extract-euler-angles-from-transformation-matrix
		float pitchX, yawY, rollZ;
		if (rotMat._11 == 1.0f || rotMat._11 == -1.0f) {
			yawY = atan2f(rotMat._13, rotMat._34);
			pitchX = 0.0f;
			rollZ = 0.0f;
		}
		else {
			yawY = atan2(-rotMat._34, rotMat._11);
			pitchX = asin(rotMat._21);
			rollZ = atan2(-rotMat._23, rotMat._22);
		}

		return DirectX::SimpleMath::Vector3(pitchX, yawY, rollZ);
	}
}

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