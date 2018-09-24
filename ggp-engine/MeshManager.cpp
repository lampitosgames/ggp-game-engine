#include "MeshManager.h"
#include "Spatial.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"

MeshManager* MeshManager::instance = nullptr;

MeshManager* MeshManager::GetInstance() {
	if (instance == nullptr) {
		instance = new MeshManager();
	}
	return instance;
}

void MeshManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void MeshManager::Start() {
	//Load default shaders
	defaultVertexShader = resourceManager->GetVertexShader(L"VertexShader.cso");
	defaultPixelShader = resourceManager->GetPixelShader(L"PixelShader.cso");
}

UINT MeshManager::AddMeshRenderer(Spatial* _gameObject) {
	MeshRenderer* tempMR = new MeshRenderer(mrUID, _gameObject);
	meshRendererUIDMap[mrUID] = tempMR;
	mrUID++;
	return mrUID - 1;
}

MeshRenderer* MeshManager::GetMeshRenderer(UINT _uniqueID) {
	auto thisMR = meshRendererUIDMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisMR != meshRendererUIDMap.end()) {
		return thisMR->second;
	}
	return nullptr;
}

void MeshManager::DeleteMeshRenderer(UINT _uniqueID) {
	MeshRenderer* mrTemp = GetMeshRenderer(_uniqueID);
	if (mrTemp) {
		delete mrTemp;
		meshRendererUIDMap.erase(_uniqueID);
	}
}

void MeshManager::Render(ID3D11DeviceContext* _dxContext, DirectX::XMFLOAT4X4 _viewMatrix, DirectX::XMFLOAT4X4 _projectionMatrix) {
	//Loop through and render every object
	std::map<UINT, MeshRenderer*>::iterator mrIterator;
	for (mrIterator = meshRendererUIDMap.begin(); mrIterator != meshRendererUIDMap.end(); ++mrIterator) {
		MeshRenderer* mrTemp = mrIterator->second;
		//If the mesh renderer doesn't have a mesh to render, continue
		if (mrTemp->GetMesh() == nullptr) { continue; }

		//Get the material's shaders
		SimpleVertexShader* vsTemp = mrTemp->GetVertexShader();
		SimplePixelShader* psTemp = mrTemp->GetPixelShader();
		//TODO: Also get texture from the material

		//Null check on all resources
		//NOTE: Commented out to demonstrate that my material system actually works
		//if (vsTemp == nullptr) { vsTemp = defaultVertexShader; }
		//if (psTemp == nullptr) { psTemp = defaultPixelShader; }

		//Upload all data to vertex shader
		vsTemp->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
		vsTemp->SetMatrix4x4("view", _viewMatrix);
		vsTemp->SetMatrix4x4("projection", _projectionMatrix);
		//TODO: Implement a way to upload color-based material data
		//TODO: Standardize what data all shaders can accept
		vsTemp->CopyAllBufferData();
		
		vsTemp->SetShader();
		psTemp->SetShader();

		mrTemp->Draw(_dxContext);
	}
}

MeshManager::MeshManager() {
	mrUID = 0;
	//Get an instance of the resource manager
	resourceManager = ResourceManager::GetInstance();
}

MeshManager::~MeshManager() {
	//Destructor is the same as clearing the singleton
	Release();
}

void MeshManager::Release() {
	//Loop through and delete every mesh renderer
	std::map<UINT, MeshRenderer*>::iterator mrIterator;
	for (mrIterator = meshRendererUIDMap.begin(); mrIterator != meshRendererUIDMap.end(); ++mrIterator) {
		MeshRenderer* mrTemp = mrIterator->second;
		if (mrTemp != nullptr) {
			delete mrTemp;
		}
	}
	//Reset mesh renderer unique ID values
	mrUID = 0;
	//Clear the map so the singleton can be reused.
	meshRendererUIDMap.clear();
}
