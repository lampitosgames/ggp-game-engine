#include "ParticleManager.h"
#include "Math.h"
#include "ResourceManager.h"

ParticleManager * ParticleManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ParticleManager();
	}
	return instance;
}

void ParticleManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void ParticleManager::Start() {
	defaultPartVS = resourceManager->GetVertexShader(L"");
	defaultPartPS = resourceManager->GetPixelShader(L"");
	//Build the particle sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//Create the sampler state
	HRESULT samplerSuccessfulLoad = ResourceManager::GetDevicePointer()->CreateSamplerState(&samplerDesc, &particleSS);
	if (samplerSuccessfulLoad != S_OK) {
		std::cout << "Sampler load error!" << std::endl;
	}

	//Create array of 4 UV coordinates. This will serve as the ENTIRE vertex buffer. All other data will come from individual
	//particles on a per-instance basis [Instanced Rendering]
	float2 uvCoords[] = { {0.0f,0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };
	//Vertex indices
	int indices[] = { 0, 1, 2, 2, 1, 3 };

	//Create a vertex buffer description for the shared particle plane
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(float2) * 4;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	//Vertex resource
	D3D11_SUBRESOURCE_DATA vertData;
	vertData.pSysMem = uvCoords;
	vertData.SysMemPitch = 0;
	vertData.SysMemSlicePitch = 0;
	//Create and store the vertex buffer
	ResourceManager::GetDevicePointer()->CreateBuffer(&vertexBufferDesc, &vertData, &partVBuffer);

	//Create an index buffer description ...
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(int) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	//Index resource
	D3D11_SUBRESOURCE_DATA indData;
	indData.pSysMem = indices;
	indData.SysMemPitch = 0;
	indData.SysMemSlicePitch = 0;


}

void ParticleManager::Update(float _dt) {}

void ParticleManager::Render() {}

ParticleEmitterID ParticleManager::AddParticleEmitter(ParticleEmitter * _particleEmitter) {
	particleEmitterUIDMap[peCount] = _particleEmitter;
	peCount++;
	return peCount - 1;
}

ParticleEmitter * ParticleManager::GetParticleEmitter(ParticleEmitterID _uniqueID) {
	auto thisPE = particleEmitterUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisPE != particleEmitterUIDMap.end()) {
		return thisPE->second;
	}
	return nullptr;
}

ParticleManager::ParticleManager() {
	peCount = 0;
	resourceManager = ResourceManager::GetInstance();
}

ParticleManager::~ParticleManager() { Release(); }

void ParticleManager::Release() {
	peCount = 0;
	particleEmitterUIDMap.clear();
	particleSS->Release();
	partVBuffer->Release();
	partIBuffer->Release();
}
