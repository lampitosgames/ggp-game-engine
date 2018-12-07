#ifndef GGP_PARTICLE_EMITTER_H
#define GGP_PARTICLE_EMITTER_H

#include <DirectXMath.h>
#include "Math.h"
#include <d3d11.h>
#include "BaseComponent.h"
class SimpleVertexShader;
class SimplePixelShader;
class GameObject;


struct ParticleData {
public:
	/*
		Particle Properties
	*/
	float3* iPos;
	float3* iVel;
	float3* accel;
	float* iRot;
	float* angularVel;

	float* startSize;
	float* endSize;

	float4* startColor;
	float4* endColor;

	float* startLife;
	float* remainLife;
	bool* alive;

	//Tracking data
	UINT particleCount;
	UINT aliveCount;

public:
	ParticleData();
	ParticleData(UINT _maxCount);
	~ParticleData();

	void generate(UINT _maxCount);
	void release();
	void kill(UINT _id);
	void wakeNext();
	void swap(UINT _id1, UINT _id2);
};

class ParticleEmitter : ECS::BaseComponent<ParticleEmitter> {
	ParticleData particles;
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
public:
	ParticleEmitter(GameObject* _gameObject);
	~ParticleEmitter();

	GameObject* gameObject;
	UINT uniqueID;
};

#endif //GGP_PARTICLE_EMITTER_H