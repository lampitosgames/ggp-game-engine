#include "stdafx.h"

#include <comdef.h> //Imports special exception class for failed HRESULTs
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "Camera.h"
#include <iostream>

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
ParticleEmitter::ParticleEmitter(GameObject* _gameObject, EmitterOptions _settings) {
	gameObject = _gameObject;
	owner = _gameObject->GetUniqueID();
	ParticleManager::GetInstance()->AddParticleEmitter(this);
	//Save the creation settings. Use functions to change them later
	settings = _settings;
	particles.generate(settings.maxParticleCount);
	forceBufferUpdate = false;
	GenerateParticleBuffers(ResourceManager::GetDevicePointer());

	while (particles.aliveCount < particles.particleCount) {
		WakeNext();
	}
}

ParticleEmitter::~ParticleEmitter() {
	for (UINT i = 1; i < bufferCount; i++) {
		buffers[i]->Release();
	}
	ParticleManager::GetInstance()->RemoveParticleEmitter(this);
}

void ParticleEmitter::Update(float _dt) {
	//Don't update all buffers unless we need to
	for (UINT i = 0; i < particles.aliveCount; i++) {
		particles.remainLife[i] -= _dt;
		if (particles.remainLife[i] <= 0.0f) {
			particles.kill(i);
			forceBufferUpdate = true;
		}
	}

	//Last thing we do
	UploadParticleBuffers(ResourceManager::GetContextPointer());
	forceBufferUpdate = false;
}

void ParticleEmitter::Render() {
	ID3D11DeviceContext* dxContext = ResourceManager::GetContextPointer();

	particleVS->SetMatrix4x4("view", RenderManager::GetInstance()->GetActiveCamera()->GetViewMatrix());
	particleVS->SetMatrix4x4("projection", RenderManager::GetInstance()->GetActiveCamera()->GetProjectionMatrix());
	particleVS->CopyAllBufferData();

	particleVS->SetShader();
	particlePS->SetShader();

	dxContext->IASetVertexBuffers(0, bufferCount, buffers, strides, offsets);
	dxContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	dxContext->DrawIndexedInstanced(6, particles.aliveCount, 0, 0, 0);
}

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
	MakeDynamicBuffer(sizeof(float3), vbSlots::BPOS, particles.iPos.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float3), vbSlots::BVEL, particles.iVel.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float3), vbSlots::BACCEL, particles.accel.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BROT, particles.iRot.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BANGULAR_VEL, particles.angularVel.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BSTART_SIZE, particles.startSize.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BEND_SIZE, particles.endSize.get(),_dxDevice);
	MakeDynamicBuffer(sizeof(float4), vbSlots::BSTART_COLOR, particles.startColor.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float4), vbSlots::BEND_COLOR, particles.endColor.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BSTART_LIFE, particles.startLife.get(), _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BREMAIN_LIFE, particles.remainLife.get(), _dxDevice);
}

void ParticleEmitter::UploadParticleBuffers(ID3D11DeviceContext * _dxContext) {
	//Only update the remainLife buffer by default
	UpdateDynamicBuffer(sizeof(float), vbSlots::BREMAIN_LIFE, particles.remainLife.get(), _dxContext);

	if (forceBufferUpdate) {
		UpdateDynamicBuffer(sizeof(float3), vbSlots::BPOS, particles.iPos.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float3), vbSlots::BVEL, particles.iVel.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float3), vbSlots::BACCEL, particles.accel.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BROT, particles.iRot.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BANGULAR_VEL, particles.angularVel.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BSTART_SIZE, particles.startSize.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BEND_SIZE, particles.endSize.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float4), vbSlots::BSTART_COLOR, particles.startColor.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float4), vbSlots::BEND_COLOR, particles.endColor.get(), _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BSTART_LIFE, particles.startLife.get(), _dxContext);
	}
}

void ParticleEmitter::MakeDynamicBuffer(UINT _itemSize, vbSlots _type, void* _initData, ID3D11Device* _dxDevice) {
	D3D11_BUFFER_DESC buffDesc = {
		_itemSize * particles.particleCount,
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_CPU_ACCESS_WRITE,
		0,
		0
	};
	strides[_type] = _itemSize;
	D3D11_SUBRESOURCE_DATA subRes;
	subRes.pSysMem = _initData;
	subRes.SysMemPitch = 0;
	subRes.SysMemSlicePitch = 0;
	ThrowIfFail(_dxDevice->CreateBuffer(&buffDesc, &subRes, &buffers[_type]));
}

void ParticleEmitter::UpdateDynamicBuffer(UINT _itemSize, vbSlots _type, void * _newData, ID3D11DeviceContext * _dxContext) {
	D3D11_MAPPED_SUBRESOURCE mapped;
	ZeroMemory(&mapped, sizeof(D3D11_MAPPED_SUBRESOURCE));
	_dxContext->Map(buffers[_type], 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, _newData, sizeof(_itemSize) * particles.aliveCount);
	_dxContext->Unmap(buffers[_type], 0);
}

void ParticleEmitter::Kill(UINT _id) {
	particles.kill(_id);
	forceBufferUpdate = true;
}

void ParticleEmitter::WakeNext() {
	particles.wakeNext();
	forceBufferUpdate = true;
	//TODO: Set the new particle's properties according to the emitter's settings
	//Currently this is just test data.
	UINT i = particles.aliveCount - 1;
	particles.iPos[i] = float3(-1.0f * i, 0.0f, 0.0f);
	particles.iVel[i] = float3(0.0f, 0.0f, 0.0f);
	particles.accel[i] = float3(0.0f, 0.0f, 0.0f);
	particles.iRot[i] = 0.0f;
	particles.angularVel[i] = 0.0f;
	particles.startSize[i] = 0.5f;
	particles.endSize[i] = 0.01f;
	particles.startColor[i] = float4(1.0f, 0.0f, 0.0f, 1.0f);
	particles.endColor[i] = float4(0.0f, 0.0f, 1.0f, 1.0f);
	particles.startLife[i] = 10.0f;
	particles.remainLife[i] = particles.startLife[i];
}
void ParticleEmitter::ThrowIfFail(HRESULT _result) {
	if (FAILED(_result)) {
		throw _com_error(_result);
	}
}
#pragma endregion
