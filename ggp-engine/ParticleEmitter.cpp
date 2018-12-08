#include "stdafx.h"

#include <comdef.h> //Imports special exception class for failed HRESULTs
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "ResourceManager.h"

#pragma region Particle Data Struct
ParticleData::ParticleData(UINT _maxCount) { generate(_maxCount); }

void ParticleData::generate(UINT _maxCount) {
	particleCount = _maxCount;
	aliveCount = 0;
	iPos.reset(new float3[_maxCount]);
	iVel.reset(new float3[_maxCount]);
	accel.reset(new float3[_maxCount]);
	iRot.reset(new float[_maxCount]);
	angularVel.reset(new float[_maxCount]);
	startSize.reset(new float[_maxCount]);
	endSize.reset(new float[_maxCount]);
	startColor.reset(new float4[_maxCount]);
	endColor.reset(new float4[_maxCount]);
	startLife.reset(new float[_maxCount]);
	remainLife.reset(new float[_maxCount]);
	alive.reset(new bool[_maxCount]);
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
//const UINT ParticleEmitter::bufferCount = 12;


ParticleEmitter::ParticleEmitter(GameObject* _gameObject, EmitterOptions _settings) {
	gameObject = _gameObject;
	owner = _gameObject->GetUniqueID();
	ParticleManager::GetInstance()->AddParticleEmitter(this);
	//Save the creation settings. Use functions to change them later
	settings = _settings;
	particles.generate(settings.maxParticleCount);
	forceBufferUpdate = false;
	GenerateParticleBuffers(ResourceManager::GetDevicePointer());
}

ParticleEmitter::~ParticleEmitter() {
	for (UINT i = 1; i < bufferCount; i++) {
		buffers[i]->Release();
	}
}

void ParticleEmitter::Update(float _dt) {
	//Don't update all buffers unless we need to
	forceBufferUpdate = false;

	//Last thing we do
	UploadParticleBuffers(ResourceManager::GetContextPointer());
}

void ParticleEmitter::Render() {}

#pragma region Get/Set
SimpleVertexShader * ParticleEmitter::GetVertexShader() { return particleVS; }
void ParticleEmitter::SetVertexShader(SimpleVertexShader * _vs) { particleVS = _vs; }
SimplePixelShader * ParticleEmitter::GetPixelShader() { return particlePS; }
void ParticleEmitter::SetPixelShader(SimplePixelShader * _ps) { particlePS = _ps; }
ID3D11SamplerState * ParticleEmitter::GetSamplerState() { return samplerState; }
void ParticleEmitter::SetSamplerState(ID3D11SamplerState * _ss) { samplerState = _ss; }
ID3D11Buffer * ParticleEmitter::GetVertexBuffer() { return buffers[vbSlots::BVERTEX_DATA]; }
void ParticleEmitter::SetVertexBuffer(ID3D11Buffer * _vBuffer) { buffers[vbSlots::BVERTEX_DATA] = _vBuffer; }
ID3D11Buffer * ParticleEmitter::GetIndexBuffer() { return indexBuffer; }
void ParticleEmitter::SetIndexBuffer(ID3D11Buffer * _iBuffer) { indexBuffer = _iBuffer; }
#pragma endregion

void ParticleEmitter::GenerateParticleBuffers(ID3D11Device * _dxDevice) {
	//Initialize the offsets array to all 0s
	for (UINT i = 0; i < bufferCount; i++) { offsets[i] = 0; }
	//Set the vertex stride
	strides[vbSlots::BVERTEX_DATA] = sizeof(float2);

	//Create a buffer for every particle property
	MakeDynamicBuffer(sizeof(float3), vbSlots::BPOS, _dxDevice);
	MakeDynamicBuffer(sizeof(float3), vbSlots::BVEL, _dxDevice);
	MakeDynamicBuffer(sizeof(float3), vbSlots::BACCEL, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BROT, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BANGULAR_VEL, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BSTART_SIZE, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BEND_SIZE, _dxDevice);
	MakeDynamicBuffer(sizeof(float4), vbSlots::BSTART_COLOR, _dxDevice);
	MakeDynamicBuffer(sizeof(float4), vbSlots::BEND_COLOR, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BSTART_LIFE, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BREMAIN_LIFE, _dxDevice);
}

void ParticleEmitter::UploadParticleBuffers(ID3D11DeviceContext * _dxContext) {
	//If no new particles were created/destroyed
	if (!forceBufferUpdate) {
		//The only buffer that needs updating is remainLife
		//Everything else is lerped on the GPU
	}
	//There were updates to the number of active particles
	else {
		//Update all buffers
	}
}

void ParticleEmitter::MakeDynamicBuffer(UINT _itemSize, vbSlots _type, ID3D11Device* _dxDevice) {
	D3D11_BUFFER_DESC buffDesc = {
		_itemSize * particles.particleCount,
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0,
		0
	};
	strides[_type] = _itemSize;
	ThrowIfFail(_dxDevice->CreateBuffer(&buffDesc, 0, &buffers[_type]));
}

void ParticleEmitter::Kill(UINT _id) {
	particles.kill(_id);
	forceBufferUpdate = true;
}

void ParticleEmitter::WakeNext() {
	particles.wakeNext();
	forceBufferUpdate = true;
	//TODO: Set the new particle's properties according to the emitter's settings
}
void ParticleEmitter::ThrowIfFail(HRESULT _result) {
	if (FAILED(_result)) {
		throw _com_error(_result);
	}
}
#pragma endregion
