#ifndef GGP_PARTICLE_EMITTER_H
#define GGP_PARTICLE_EMITTER_H

#include "stdafx.h"

#include <DirectXMath.h>
#include "Math.h"
#include <d3d11.h>
#include <random>
#include "BaseComponent.h"
class SimpleVertexShader;
class SimplePixelShader;
class GameObject;
class Texture;


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
	float3* iPos = nullptr;	        //Initial position. Final pos calculated on GPU with kinematics
	float3* iVel = nullptr;	        //Initial velocity. Final vel ...
	float3* accel = nullptr;	    //Const Accel. This can't change per-particle, but it can be different per-particle
	float* iRot = nullptr;          //Initial rotation (in radians) of the particle's quad
	float* angularVel = nullptr;   	//Angular velocity (in radians)
	float* startSize = nullptr;     //Initial scale of particle
	float* endSize = nullptr;       //Final scale of particle
	float4* startColor = nullptr;   //Initial color of particle
	float4* endColor = nullptr;     //Final color of particle
	float* startLife = nullptr;     //Length of particle's life (seconds)
	float* remainLife = nullptr;    //Currently remaining time in this particle's life (seconds)
	//Alive is the only one without a buffer
	bool* alive = nullptr;          //Is the particle active?

	//Tracking data
	UINT particleCount;
	UINT aliveCount;

public:
	ParticleData() {}
	ParticleData(UINT _maxCount);
	~ParticleData() { release(); }

	void generate(UINT _maxCount);
	void kill(UINT _id);
	void wakeNext();
	void swap(UINT _id1, UINT _id2);
private:
	void release();
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
public:
	UINT maxParticleCount; //Maximum number of particles the emitter can generate at once
	float startDelay;      //How long does this emitter have to be alive before it begins emitting particles
	float duration;        //Length of time the system runs
	float emissionRate;    //Seconds between emission of each particle
	bool looping;          //If true, emitter's duration is reset once it hits 0
	bool playing;          //If true, the particle emitter is currently emitting
	UINT hasTexture;       //Using a texture?
	LPCWSTR textureFilepath; //Filepath to particle texture
	bool useDepthSettings; //Use additive blending with depth turned off

	//Emission shape props
	enum emitterShape {
		//Emit particles from a random position inside of a sphere, moving out from the sphere's center. The sphere is centered on 0,0,0
		SPHERE = 0,
		//Emit particles from the base of a cone, with randomized initial velocities pointing towards the cone's base. The cone's tip is at 0,0,0 and it is aligned along the local Z axis 
		CONE = 1,
		//Emit particles from a random position inside a cube volume with side length radius*2. Particles move in the gameObject's +z direction
		CUBE = 2,
		//Emit particles from a random position inside a cylinder volume. Particles move in the gameObject's +z direction
		CYLINDER = 3
	};
	emitterShape shape; //Shape of emission
	float radius; //Used for all 3 current emission types
	float angle; //Angle at the tip of the cone
	float height; //Height of cylinder

	//Particle properties
	float partLifetime;     //Lifetime of individual particles
	float partInitialSpeed; //Initial particle speed. Actual velocity is determined by emission shape props
	float3 partAccel;       //Acceleration vector
	bool partAccelLSpace;   //Is the acceleration in local space?
	float partInitialRot;   //Initial rotation of each particle
	float partAngularVel;   //Angular velocity for rotation
	bool partRandomRotDir;  //Randomize the +/- sign of the angular velocity on a per-particle basis
	float partStartSize;    //Initial particle size
	float partEndSize;      //Final particle size
	float4 partStartColor;  //Initial particle color (Should be (1,1,1) if using texture)
	float4 partEndColor;    //Final particle color
public:
	EmitterOptions();
	~EmitterOptions() {};
};

class ParticleEmitter : public ECS::BaseComponent<ParticleEmitter> {
private:
	//Struct holding all of the particle data in a bunch of arrays
	ParticleData particles;
	EmitterOptions settings;
	DirectX::XMFLOAT4X4 worldMatRaw;
	float totalPlayTime;
	UINT totalSpawned;

	//DirectX Resources
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	ID3D11SamplerState* samplerState;
	Texture* particleTexture;

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
	void Render(ID3D11BlendState* _blend, ID3D11DepthStencilState* _depth);

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

	//Helper methods for creating/updating all of the instance buffers
	void MakeDynamicBuffer(UINT _itemSize, vbSlots _type, void* _initData, ID3D11Device* _dxDevice);
	void UpdateDynamicBuffer(UINT _itemSize, vbSlots _type, void* _newData, ID3D11DeviceContext* _dxContext);

	//Kill a particle
	void Kill(UINT _id);
	//Wake a new particle
	void WakeNext();

	//Throw an exception if creation of a resource doesn't work
	void ThrowIfFail(HRESULT _result);
};

#endif //GGP_PARTICLE_EMITTER_H