#include "stdafx.h"


#include "ParticleManager.h"
#include "Math.h"
#include "ResourceManager.h"

ParticleManager* ParticleManager::instance = nullptr;

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

void ParticleManager::Init() {
	//Map all of our buffers to what the vertex shader expects as input
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		//PER-VERTEX
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, vbSlots::BVERTEX_DATA, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//PER-INSTANCE
		{"INITIAL_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, vbSlots::BPOS, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INITIAL_VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, vbSlots::BVEL, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"ACCELERATION", 0, DXGI_FORMAT_R32G32B32_FLOAT, vbSlots::BACCEL, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INITIAL_ROTATION", 0, DXGI_FORMAT_R32_FLOAT, vbSlots::BROT, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"ANGULAR_VELOCITY", 0, DXGI_FORMAT_R32_FLOAT, vbSlots::BANGULAR_VEL, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"START_SIZE", 0, DXGI_FORMAT_R32_FLOAT, vbSlots::BSTART_SIZE, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"END_SIZE", 0, DXGI_FORMAT_R32_FLOAT, vbSlots::BEND_SIZE, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"START_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, vbSlots::BSTART_COLOR, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"END_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, vbSlots::BEND_COLOR, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"START_LIFE", 0, DXGI_FORMAT_R32_FLOAT, vbSlots::BSTART_LIFE, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"REMAIN_LIFE", 0, DXGI_FORMAT_R32_FLOAT, vbSlots::BREMAIN_LIFE, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};
	//Create default particle shaders
	defaultPartVS = resourceManager->GetVertexShader(L"ParticleVertexShader.cso");
	defaultPartPS = resourceManager->GetPixelShader(L"ParticlePixelShader.cso");
	//Create the actual input layout resource and pass it to the vertex shader
	HRESULT asdfresult = ResourceManager::GetDevicePointer()->CreateInputLayout(&layout[0], (UINT)layout.size(), defaultPartVS->GetShaderBlob()->GetBufferPointer(), defaultPartVS->GetShaderBlob()->GetBufferSize(), &inputLayout);
	defaultPartVS->SetInputLayout(inputLayout, true);

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

	//Build the particle blend state
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT blendSuccessfulLoad = ResourceManager::GetDevicePointer()->CreateBlendState(&blendDesc, &particleBS);
	if (blendSuccessfulLoad != S_OK) {
		std::cout << "Blend state load error!" << std::endl;
	}

	//Build the particle depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ResourceManager::GetDevicePointer()->CreateDepthStencilState(&depthDesc, &particleDS);

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
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	//Vertex resource
	D3D11_SUBRESOURCE_DATA vertData;
	vertData.pSysMem = uvCoords;
	vertData.SysMemPitch = 0;
	vertData.SysMemSlicePitch = 0;
	//Create and store the vertex buffer
	HRESULT successVBCreate = ResourceManager::GetDevicePointer()->CreateBuffer(&vertexBufferDesc, &vertData, &partVBuffer);

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
	//Create and store the index buffer
	ResourceManager::GetDevicePointer()->CreateBuffer(&indexBufferDesc, &indData, &partIBuffer);
}

void ParticleManager::Update(float _dt) {
	//Loop through every particle emitter
	std::map<ParticleEmitterID, ParticleEmitter*>::iterator peIterator;
	for (peIterator = particleEmitterUIDMap.begin(); peIterator != particleEmitterUIDMap.end();) {
		ParticleEmitter* peTemp = peIterator->second;
		//Make sure it hasn't been cleaned up by the component system
		if (peTemp == nullptr) {
			particleEmitterUIDMap.erase(peIterator++);
			continue;
		}
		else {
			peIterator++;
		}
		peTemp->Update(_dt);
	}
}

void ParticleManager::Render() {
	//Loop through every particle emitter
	std::map<ParticleEmitterID, ParticleEmitter*>::iterator peIterator;
	for (peIterator = particleEmitterUIDMap.begin(); peIterator != particleEmitterUIDMap.end();) {
		ParticleEmitter* peTemp = peIterator->second;
		//Make sure it hasn't been cleaned up by the component system
		if (peTemp == nullptr) {
			particleEmitterUIDMap.erase(peIterator++);
			continue;
		}
		else {
			peIterator++;
		}
		peTemp->Render(particleBS, particleDS);
	}
}

ID3D11SamplerState * ParticleManager::GetParticleSamplerState() { return particleSS; }
ID3D11InputLayout * ParticleManager::GetInputLayout() { return inputLayout; }
ID3D11Buffer * ParticleManager::GetParticleVertexBuffer() { return partVBuffer; }
ID3D11Buffer * ParticleManager::GetParticleIndexBuffer() { return partIBuffer; }

ParticleEmitterID ParticleManager::AddParticleEmitter(ParticleEmitter * _particleEmitter) {
	//Inject the particle emitter with DirectX resources
	_particleEmitter->SetVertexShader(defaultPartVS);
	_particleEmitter->SetPixelShader(defaultPartPS);
	_particleEmitter->SetSamplerState(particleSS);
	_particleEmitter->SetVertexBuffer(partVBuffer);
	_particleEmitter->SetIndexBuffer(partIBuffer);
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

void ParticleManager::RemoveParticleEmitter(ParticleEmitter * _particleEmitter) {
	auto peIt = particleEmitterUIDMap.begin();
	for (; peIt != particleEmitterUIDMap.end(); ++peIt) {
		if (peIt->second == _particleEmitter) {
			particleEmitterUIDMap[peIt->first] = nullptr;
		}
	}
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
	particleBS->Release();
	particleDS->Release();
	partVBuffer->Release();
	partIBuffer->Release();
}
