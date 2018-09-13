#include "MeshManager.h"
#include "Spatial.h"
#include "MeshRenderer.h"

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

UINT MeshManager::AddMeshRenderer(Spatial* _gameObject) {
	MeshRenderer* tempMR = new MeshRenderer(mrUID, _gameObject);
	meshRendererMap[mrUID] = tempMR;
	mrUID++;
	return mrUID - 1;
}

MeshRenderer* MeshManager::GetMeshRenderer(UINT _uniqueID) {
	auto thisMR = meshRendererMap.find(_uniqueID);
	//If found, return it.  Else, return nullptr
	if (thisMR != meshRendererMap.end()) {
		return thisMR->second;
	}
	return nullptr;
}

void MeshManager::DeleteMeshRenderer(UINT _uniqueID) {
	MeshRenderer* mrTemp = GetMeshRenderer(_uniqueID);
	if (mrTemp) {
		delete mrTemp;
		meshRendererMap.erase(_uniqueID);
	}
}

void MeshManager::Render(ID3D11DeviceContext* _dxContext, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, DirectX::XMFLOAT4X4 _viewMatrix, DirectX::XMFLOAT4X4 _projectionMatrix) {
	//Loop through and render every object
	std::map<UINT, MeshRenderer*>::iterator mrIterator;
	for (mrIterator = meshRendererMap.begin(); mrIterator != meshRendererMap.end(); ++mrIterator) {
		MeshRenderer* mrTemp = mrIterator->second;
		_vertexShader->SetMatrix4x4("world", mrTemp->GetWorldMatrix());
		_vertexShader->SetMatrix4x4("view", _viewMatrix);
		_vertexShader->SetMatrix4x4("projection", _projectionMatrix);
		_vertexShader->CopyAllBufferData();
		
		_vertexShader->SetShader();
		_pixelShader->SetShader();

		mrTemp->Draw(_dxContext);
	}
}

MeshManager::MeshManager() {
	mrUID = 0;
}

MeshManager::~MeshManager() {
	//Destructor is the same as clearing the singleton
	Release();
}

void MeshManager::Release() {
	//Loop through and delete every mesh renderer
	std::map<UINT, MeshRenderer*>::iterator mrIterator;
	for (mrIterator = meshRendererMap.begin(); mrIterator != meshRendererMap.end(); ++mrIterator) {
		MeshRenderer* mrTemp = mrIterator->second;
		if (mrTemp != nullptr) {
			delete mrTemp;
		}
	}
	//Reset mesh renderer unique ID values
	mrUID = 0;
	//Clear the map so the singleton can be reused.
	meshRendererMap.clear();
}
