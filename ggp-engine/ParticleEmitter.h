#ifndef GGP_PARTICLE_EMITTER_H
#define GGP_PARTICLE_EMITTER_H

#include <DirectXMath.h>
#include "Math.h"
#include <d3d11.h>
#include <memory>
#include "BaseComponent.h"
class SimpleVertexShader;
class SimplePixelShader;
class GameObject;


struct ParticleData {
public:
	/*
		Particle Properties
		Adding one means you need to add the following to the ParticleEmitter:
		- a new entry in the vbSlots enum
		- increase the bufferCount by 1
		- creation code in GenerateParticleBuffers()
		- update code in UpdateParticleBuffers()
		- update the shader input layout
	*/
	std::unique_ptr<float3[]> iPos;	        //Initial position. Final pos calculated on GPU with kinematics
	std::unique_ptr<float3[]> iVel;	        //Initial velocity. Final vel ...
	std::unique_ptr<float3[]> accel;	    //Const Accel. This can't change per-particle, but it can be different per-particle
	std::unique_ptr<float[]> iRot;          //Initial rotation (in radians) of the particle's quad
	std::unique_ptr<float[]> angularVel;	//Angular velocity (in radians)
	std::unique_ptr<float[]> startSize;     //Initial scale of particle
	std::unique_ptr<float[]> endSize;       //Final scale of particle
	std::unique_ptr<float4[]> startColor;   //Initial color of particle
	std::unique_ptr<float4[]> endColor;     //Final color of particle
	std::unique_ptr<float[]> startLife;     //Length of particle's life (seconds)
	std::unique_ptr<float[]> remainLife;    //Currently remaining time in this particle's life (seconds)

	//Alive is the only one without a buffer
	std::unique_ptr<bool[]> alive;          //Is the particle active?

	//Tracking data
	UINT particleCount;
	UINT aliveCount;

public:
	ParticleData() {}
	ParticleData(UINT _maxCount);
	~ParticleData() {}

	void generate(UINT _maxCount);
	void kill(UINT _id);
	void wakeNext();
	void swap(UINT _id1, UINT _id2);
};


enum vbSlots {
	BVERTEX_DATA = 0,
	BPOS = 1,
	BVEL = 2,
	BACCEL = 3,
	BROT = 4,
	BANGULAR_VEL = 5,
	BSTART_SIZE = 6,
	BEND_SIZE = 7,
	BSTART_COLOR = 8,
	BEND_COLOR = 9,
	BSTART_LIFE = 10,
	BREMAIN_LIFE = 11
};

struct EmitterOptions {
	UINT maxParticleCount; //Maximum number of particles the emitter can generate at once
};

class ParticleEmitter : public ECS::BaseComponent<ParticleEmitter> {
private:
	//Struct holding all of the particle data in a bunch of arrays
	ParticleData particles;
	EmitterOptions settings;

	//DirectX Resources
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	ID3D11SamplerState* samplerState;

	static const UINT bufferCount = 12;
	//Array containing the vertex buffer at [0], and all the particle instance buffers
	ID3D11Buffer* buffers[bufferCount];
	//Stirde and offset arrays for every buffer. (Offset will always be 0 in our case)
	UINT strides[bufferCount];
	UINT offsets[bufferCount];

	//Index buffer
	ID3D11Buffer* indexBuffer;

	//If true when UpdateParticleBuffers is called, forces every buffer to update
	//Should be set to true whenever a particle is killed or created
	bool forceBufferUpdate;
public:
	ParticleEmitter(GameObject* _gameObject, EmitterOptions _settings);
	~ParticleEmitter();

	void Update(float _dt);
	void Render();

	/*
		GET/SET
	*/
	SimpleVertexShader* GetVertexShader();
	void SetVertexShader(SimpleVertexShader* _vs);
	SimplePixelShader* GetPixelShader();
	void SetPixelShader(SimplePixelShader* _ps);
	ID3D11SamplerState* GetSamplerState();
	void SetSamplerState(ID3D11SamplerState* _ss);
	ID3D11Buffer* GetVertexBuffer();
	void SetVertexBuffer(ID3D11Buffer* _vBuffer);
	ID3D11Buffer* GetIndexBuffer();
	void SetIndexBuffer(ID3D11Buffer* _iBuffer);

	/*
		Emitter Settings Management
	*/
	//TODO: Ability to change options in the EmitterOptions struct
	//Can't edit directly because there might be extra logic tied to changing
	//certain settings

	GameObject* gameObject;
private:
	void GenerateParticleBuffers(ID3D11Device* _dxDevice);
	void UploadParticleBuffers(ID3D11DeviceContext* _dxContext);

	//Helper method for creating all of the instance buffers
	void MakeDynamicBuffer(UINT _itemSize, vbSlots _type, ID3D11Device* _dxDevice);

	//Kill a particle
	void Kill(UINT _id);
	//Wake a new particle
	void WakeNext();

	//Throw an exception if creation of a resource doesn't work
	void ThrowIfFail(HRESULT _result);
};

#endif //GGP_PARTICLE_EMITTER_H