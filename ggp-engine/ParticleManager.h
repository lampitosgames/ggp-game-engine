#ifndef GGP_PARTICLE_MANAGER_H
#define GGP_PARTICLE_MANAGER_H


#include <map>
#include "SimpleShader.h"
#include "ParticleEmitter.h"
class ResourceManager;

typedef UINT ParticleEmitterID;

class ParticleManager {
	static ParticleManager* instance;

	ResourceManager* resourceManager;

	//Shared DirectX resources
	SimpleVertexShader* defaultPartVS;
	SimplePixelShader* defaultPartPS;
	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* particleSS;
	ID3D11BlendState* particleBS;
	ID3D11DepthStencilState* particleDS;
	ID3D11Buffer* partVBuffer;
	ID3D11Buffer* partIBuffer;

	//Tracking for particle emitter components
	ParticleEmitterID peCount;
	std::map<ParticleEmitterID, ParticleEmitter*> particleEmitterUIDMap;
public:
	static ParticleManager* GetInstance();
	static void ReleaseInstance();

	ParticleManager(ParticleManager const&) = delete;
	void operator=(ParticleManager const&) = delete;

	void Init();

	void Update(float _dt);

	void Render();

	/*
		GET/SET
	*/
	ID3D11SamplerState* GetParticleSamplerState();
	ID3D11InputLayout* GetInputLayout();
	ID3D11Buffer* GetParticleVertexBuffer();
	ID3D11Buffer* GetParticleIndexBuffer();

	/*
		PARTICLE EMITTER HELPERS
		Generates particles at an object's position. Lots of properties
	*/
	ParticleEmitterID AddParticleEmitter(ParticleEmitter* _particleEmitter);
	ParticleEmitter* GetParticleEmitter(ParticleEmitterID _uniqueID);
	void RemoveParticleEmitter(ParticleEmitter* _particleEmitter);
private:
	ParticleManager();
	~ParticleManager();
	void Release();
};

#endif //GGP_PARTICLE_MANAGER_H