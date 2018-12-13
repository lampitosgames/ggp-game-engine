#include "stdafx.h"

#include <comdef.h> //Imports special exception class for failed HRESULTs
#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "GameObject.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "Texture.h"

using namespace DirectX;

#pragma region Particle Data Struct
ParticleData::ParticleData(UINT _maxCount) { generate(_maxCount); }

void ParticleData::generate(UINT _maxCount) {
	particleCount = _maxCount;
	aliveCount = 0;
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

void ParticleData::release() {
	if (iPos != nullptr) { delete[] iPos; iPos = nullptr; }
	if (iVel != nullptr) { delete[] iVel; iVel = nullptr; }
	if (accel != nullptr) { delete[] accel; accel = nullptr; }
	if (iRot != nullptr) { delete[] iRot; iRot = nullptr; }
	if (angularVel != nullptr) { delete[] angularVel; angularVel = nullptr; }
	if (startSize != nullptr) { delete[] startSize; startSize = nullptr; }
	if (endSize != nullptr) { delete[] endSize; endSize = nullptr; }
	if (startColor != nullptr) { delete[] startColor; startColor = nullptr; }
	if (endColor != nullptr) { delete[] endColor; endColor = nullptr; }
	if (startLife != nullptr) { delete[] startLife; startLife = nullptr; }
	if (remainLife != nullptr) { delete[] remainLife; remainLife = nullptr; }
	if (alive != nullptr) { delete[] alive; alive = nullptr; }
}
#pragma endregion


#pragma region Emitter Options
EmitterOptions::EmitterOptions() {
	maxParticleCount = 2048;
	startDelay = 0.0f;
	duration = 5.0f;
	emissionRate = 0.0001f;
	looping = true;
	playing = true;
	hasTexture = 1;
	textureFilepath = L"assets/textures/particles/particle.jpg";
	useDepthSettings = true;
	shape = emitterShape::CUBE;
	radius = 0.2f;
	angle = 25.0f;
	height = 0.5f;
	partLifetime = 5.0f;
	partInitialSpeed = 0.5f;
	partAccel = float3(0.0f, 0.0f, 0.0f);
	partAccelLSpace = true;
	partInitialRot = 0.0f;
	partAngularVel = 0.0f;
	partRandomRotDir = true;
	partStartSize = 0.1f;
	partEndSize = 0.5f;
	partStartColor = float4(1.0f, 0.1f, 0.1f, 0.3f);
	partEndColor = float4(1.0f, 0.6f, 0.1f, 0.0f);
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
	//Update world matrix
	worldMatRaw = gameObject->transform.GetWorldMatrix(false);
	//Init variables
	totalPlayTime = 0.0f;
	totalSpawned = 0;
	if (settings.hasTexture) {
		particleTexture = ResourceManager::GetInstance()->GetTexture(settings.textureFilepath);
	}
	else {
		particleTexture = nullptr;
	}
}

ParticleEmitter::~ParticleEmitter() {
	for (UINT i = 1; i < bufferCount; i++) {
		buffers[i]->Release();
	}
	ParticleManager::GetInstance()->RemoveParticleEmitter(this);
}

void ParticleEmitter::Update(float _dt) {
	//Only update if playing
	if (!settings.playing) { return; }

	//Reduce remaining duration by the time delta
	totalPlayTime += _dt;
	//If the duration has expired
	if (totalPlayTime >= settings.duration && !settings.looping) {
			totalPlayTime = 0;
			settings.playing = false;
			return;
	}

	//Update world matrix
	worldMatRaw = gameObject->transform.GetWorldMatrix(false);

	for (UINT i = 0; i < particles.aliveCount; i++) {
		particles.remainLife[i] -= _dt;
		if (particles.remainLife[i] <= 0.0f) {
			particles.kill(i);
			forceBufferUpdate = true;
		}
	}

	//If we can spawn more particles
	if (particles.aliveCount < particles.particleCount) {
		if (settings.emissionRate * totalSpawned + settings.startDelay < totalPlayTime) {
			WakeNext();
		}
	}

	//Last thing we do
	UploadParticleBuffers(ResourceManager::GetContextPointer());
	forceBufferUpdate = false;
}

void ParticleEmitter::Render(ID3D11BlendState* _blend, ID3D11DepthStencilState* _depth) {
	ID3D11DeviceContext* dxContext = ResourceManager::GetContextPointer();

	float blend[4] = { 1,1,1,1 };
	if (settings.useDepthSettings) {
		dxContext->OMSetBlendState(_blend, blend, 0xffffffff);
		dxContext->OMSetDepthStencilState(_depth, 0);
	}

	particleVS->SetMatrix4x4("view", RenderManager::GetInstance()->GetActiveCamera()->GetViewMatrix());
	particleVS->SetMatrix4x4("projection", RenderManager::GetInstance()->GetActiveCamera()->GetProjectionMatrix());
	particleVS->CopyAllBufferData();

	particlePS->SetData("hasTexture", &settings.hasTexture, sizeof(int));
	if (settings.hasTexture) {
		particlePS->SetSamplerState("trilinear", samplerState);
		particlePS->SetShaderResourceView("partTexture", particleTexture->GetShaderResourceView());
	}
	particlePS->CopyAllBufferData();

	particleVS->SetShader();
	particlePS->SetShader();

	dxContext->IASetVertexBuffers(0, bufferCount, buffers, strides, offsets);
	dxContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	dxContext->DrawIndexedInstanced(6, particles.aliveCount, 0, 0, 0);

	if (settings.useDepthSettings) {
		dxContext->OMSetBlendState(0, blend, 0xffffffff);
		dxContext->OMSetDepthStencilState(0, 0);
	}
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
	MakeDynamicBuffer(sizeof(float3), vbSlots::BPOS, particles.iPos, _dxDevice);
	MakeDynamicBuffer(sizeof(float3), vbSlots::BVEL, particles.iVel, _dxDevice);
	MakeDynamicBuffer(sizeof(float3), vbSlots::BACCEL, particles.accel, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BROT, particles.iRot, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BANGULAR_VEL, particles.angularVel, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BSTART_SIZE, particles.startSize, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BEND_SIZE, particles.endSize,_dxDevice);
	MakeDynamicBuffer(sizeof(float4), vbSlots::BSTART_COLOR, particles.startColor, _dxDevice);
	MakeDynamicBuffer(sizeof(float4), vbSlots::BEND_COLOR, particles.endColor, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BSTART_LIFE, particles.startLife, _dxDevice);
	MakeDynamicBuffer(sizeof(float), vbSlots::BREMAIN_LIFE, particles.remainLife, _dxDevice);
}

void ParticleEmitter::UploadParticleBuffers(ID3D11DeviceContext * _dxContext) {
	//Only update the remainLife buffer by default
	UpdateDynamicBuffer(sizeof(float), vbSlots::BREMAIN_LIFE, particles.remainLife, _dxContext);

	if (forceBufferUpdate) {
		UpdateDynamicBuffer(sizeof(float3), vbSlots::BPOS, particles.iPos, _dxContext);
		UpdateDynamicBuffer(sizeof(float3), vbSlots::BVEL, particles.iVel, _dxContext);
		UpdateDynamicBuffer(sizeof(float3), vbSlots::BACCEL, particles.accel, _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BROT, particles.iRot, _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BANGULAR_VEL, particles.angularVel, _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BSTART_SIZE, particles.startSize, _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BEND_SIZE, particles.endSize, _dxContext);
		UpdateDynamicBuffer(sizeof(float4), vbSlots::BSTART_COLOR, particles.startColor, _dxContext);
		UpdateDynamicBuffer(sizeof(float4), vbSlots::BEND_COLOR, particles.endColor, _dxContext);
		UpdateDynamicBuffer(sizeof(float), vbSlots::BSTART_LIFE, particles.startLife, _dxContext);
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
	memcpy(mapped.pData, _newData, _itemSize * particles.aliveCount);
	_dxContext->Unmap(buffers[_type], 0);
}

void ParticleEmitter::Kill(UINT _id) {
	particles.kill(_id);
	forceBufferUpdate = true;
}

void ParticleEmitter::WakeNext() {
	particles.wakeNext();
	totalSpawned += 1;
	forceBufferUpdate = true;

	//Get new particle index
	UINT i = particles.aliveCount - 1;


	//Create position based on emission type
	float ix; float iy; float iz;
	switch (settings.shape) {
	case EmitterOptions::emitterShape::SPHERE:
		//Random position in a sphere
		do {
			ix = ((float)(rand() % (UINT)(settings.radius * 2 * 1000)) / 1000.0f) - settings.radius;
			iy = ((float)(rand() % (UINT)(settings.radius * 2 * 1000)) / 1000.0f) - settings.radius;
			iz = ((float)(rand() % (UINT)(settings.radius * 2 * 1000)) / 1000.0f) - settings.radius;
		} while (ix*ix + iy*iy + iz*iz > settings.radius * settings.radius);
		particles.iPos[i] = float3(ix, iy, iz);
		particles.iVel[i] = particles.iPos[i];
		XMStoreFloat3(&particles.iVel[i], XMVectorScale(
			XMVector3Normalize(XMLoadFloat3(&particles.iVel[i])),
			settings.partInitialSpeed
		));
		break;
	case EmitterOptions::emitterShape::CUBE:
		ix = ((float)(rand() % (UINT)(settings.radius * 2 * 1000)) / 1000.0f) - settings.radius;
		iy = ((float)(rand() % (UINT)(settings.radius * 2 * 1000)) / 1000.0f) - settings.radius;
		iz = ((float)(rand() % (UINT)(settings.radius * 2 * 1000)) / 1000.0f) - settings.radius;
		particles.iPos[i] = float3(ix, iy, iz);
		particles.iVel[i] = float3(0.0f, 0.0f, settings.partInitialSpeed);
		break;
	case EmitterOptions::emitterShape::CYLINDER:
	case EmitterOptions::emitterShape::CONE:
	default:
		//Default to spawning every particle from the center with a velocity of +speed on the z axis
		particles.iPos[i] = float3(0.0f, 0.0f, 0.0f);
		particles.iVel[i] = float3(0.0f, 0.0f, settings.partInitialSpeed);
		break;
	}
	//Load world matrix
	XMMATRIX worldMat = XMLoadFloat4x4(&worldMatRaw);
	//Transform position into world space
	XMVECTOR localPos = XMLoadFloat3(&particles.iPos[i]);
	XMStoreFloat3(&particles.iPos[i], XMVector3Transform(localPos, worldMat));
	//Transform velocity into world space
	XMMATRIX rotMat = XMLoadFloat4x4(&gameObject->transform.GetRotationMatrix());
	XMVECTOR localVel = XMLoadFloat3(&particles.iVel[i]);
	XMStoreFloat3(&particles.iVel[i], XMVector3Transform(localVel, rotMat));
	//Set constant acceleration
	particles.accel[i] = settings.partAccel;
	//If acceleration is in local space
	if (settings.partAccelLSpace && (particles.accel[i].x != 0 || particles.accel[i].y != 0 || particles.accel[i].z != 0)) {
		//Transform by the world matrix
		XMVECTOR localAccel = XMLoadFloat3(&particles.accel[i]);
		XMStoreFloat3(&particles.accel[i], XMVector3Transform(localAccel, worldMat));
	}

	//Set initial rotation amount
	particles.iRot[i] = settings.partInitialRot;
	//Set angular velocity
	particles.angularVel[i] = settings.partAngularVel;
	//Randomize rotation direction?
	if (settings.partRandomRotDir) {
		particles.angularVel[i] *= rand() % 2 == 0 ? 1 : -1;
	}

	//Grab remaining props directly from the EmitterOptions struct
	particles.startSize[i] = settings.partStartSize;
	particles.endSize[i] = settings.partEndSize;
	particles.startColor[i] = settings.partStartColor;
	particles.endColor[i] = settings.partEndColor;
	particles.startLife[i] = settings.partLifetime;
	particles.remainLife[i] = particles.startLife[i];
}
void ParticleEmitter::ThrowIfFail(HRESULT _result) {
	if (FAILED(_result)) {
		throw _com_error(_result);
	}
}
#pragma endregion
