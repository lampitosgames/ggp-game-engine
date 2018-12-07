#include "stdafx.h"

#include "ParticleEmitter.h"

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
	startLifetime = new float[_maxCount];
	remainingLifetime = new float[_maxCount];
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
	delete[] startLifetime;
	delete[] remainingLifetime;
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
	std::swap(startLifetime[_id1], startLifetime[_id2]);
	std::swap(remainingLifetime[_id1], remainingLifetime[_id2]);
	std::swap(alive[_id1], alive[_id2]);
}

ParticleEmitter::ParticleEmitter(GameObject* _gameObject) {}

ParticleEmitter::~ParticleEmitter() {}
