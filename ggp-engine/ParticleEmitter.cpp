#include "stdafx.h"

#include "ParticleEmitter.h"
#include "GameObject.h"
#include "RenderManager.h"



#pragma region Particle Data Struct
ParticleData::ParticleData() { generate(512); }

ParticleData::ParticleData(UINT _maxCount) { generate(_maxCount); }

ParticleData::~ParticleData() {
	release();
}

void ParticleData::generate(UINT _maxCount) {
	particleCount = _maxCount;
	aliveCount = 0;

	//release old arrays
	release();

	iPos = new float3[_maxCount];
	iVel = new float3[_maxCount];
	accel = new float3[_maxCount];
	iRot = new float[_maxCount];
	angularVel = new float[_maxCount];
	startSize = new float[_maxCount];
	endSize = new float[_maxCount];
	startColor = new float4[_maxCount];
	endColor = new float4[_maxCount];
	startLife = new float[_maxCount];
	remainLife = new float[_maxCount];
	alive = new bool[_maxCount];
}

void ParticleData::release() {
	delete[] iPos;
	delete[] iVel;
	delete[] accel;
	delete[] iRot;
	delete[] angularVel;
	delete[] startSize;
	delete[] endSize;
	delete[] startColor;
	delete[] endColor;
	delete[] startLife;
	delete[] remainLife;
	delete[] alive;
}

void ParticleData::kill(UINT _id) {
	alive[_id] = false;
	aliveCount--;
	swap(_id, aliveCount);
}

void ParticleData::wakeNext() {
	alive[aliveCount++] = true;
}

void ParticleData::swap(UINT _id1, UINT _id2) {
	std::swap(iPos[_id1], iPos[_id2]);
	std::swap(iVel[_id1], iVel[_id2]);
	std::swap(accel[_id1], accel[_id2]);
	std::swap(iRot[_id1], iRot[_id2]);
	std::swap(angularVel[_id1], angularVel[_id2]);
	std::swap(startSize[_id1], startSize[_id2]);
	std::swap(endSize[_id1], endSize[_id2]);
	std::swap(startColor[_id1], startColor[_id2]);
	std::swap(endColor[_id1], endColor[_id2]);
	std::swap(startLife[_id1], startLife[_id2]);
	std::swap(remainLife[_id1], remainLife[_id2]);
	std::swap(alive[_id1], alive[_id2]);
}
#pragma endregion

#pragma region Particle Emitter Component
ParticleEmitter::ParticleEmitter(GameObject* _gameObject) {
	gameObject = _gameObject;
	owner = _gameObject->GetUniqueID();
}

ParticleEmitter::~ParticleEmitter() {}
#pragma endregion
