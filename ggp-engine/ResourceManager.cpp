#include "ResourceManager.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Mesh.h"

using namespace DirectX;
using namespace std;

ResourceManager* ResourceManager::instance = nullptr;
ID3D11Device* ResourceManager::dxDevice = nullptr;
ID3D11DeviceContext* ResourceManager::dxContext = nullptr;

ResourceManager* ResourceManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ResourceManager();
	}
	return instance;
}

void ResourceManager::ReleaseInstance() {
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void ResourceManager::SetDevicePointer(ID3D11Device* _dxDevice) {
	ResourceManager::dxDevice = _dxDevice;
}

void ResourceManager::SetContextPointer(ID3D11DeviceContext* _dxContext) {
	ResourceManager::dxContext = _dxContext;
}

Material* ResourceManager::GetMaterial(string _uniqueID) {
	//First, look up this material.  If it exists, just return it
	auto thisMaterial = materialUIDMap.find(_uniqueID);
	if (thisMaterial != materialUIDMap.end()) {
		return thisMaterial->second;
	}
	//The material doesn't exist, so create a new default material
	Material* newMaterial = new Material(_uniqueID);
	//Add it to the map
	materialUIDMap[_uniqueID] = newMaterial;
	//Return new material
	return newMaterial;
}

Material* ResourceManager::AddMaterial(string _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, XMFLOAT4 _color) {
	//First, look up this material.  If it exists, just return it
	auto thisMaterial = materialUIDMap.find(_uniqueID);
	if (thisMaterial != materialUIDMap.end()) {
		return thisMaterial->second;
	}
	//Create new material with shaders
	Material* newMaterial = new Material(_uniqueID, _vertexShader, _pixelShader, _color);
	//Add it to the map
	materialUIDMap[_uniqueID] = newMaterial;
	//Return new material
	return newMaterial;
}

Material* ResourceManager::AddMaterial(string _uniqueID, LPCWSTR _vertexFilestring, LPCWSTR _pixelFilestring, XMFLOAT4 _color) {
	//First, look up this material.  If it exists, just return it
	auto thisMaterial = materialUIDMap.find(_uniqueID);
	if (thisMaterial != materialUIDMap.end()) {
		return thisMaterial->second;
	}
	//Create a new material. Fetch shaders inline
	Material* newMaterial = new Material(_uniqueID, GetVertexShader(_vertexFilestring), GetPixelShader(_pixelFilestring), _color);
	//Add it to the map
	materialUIDMap[_uniqueID] = newMaterial;
	//Return new material
	return newMaterial;
}

Material* ResourceManager::AddMaterial(string _uniqueID, DirectX::XMFLOAT4 _color) {
	//First, look up this material.  If it exists, just return it
	auto thisMaterial = materialUIDMap.find(_uniqueID);
	if (thisMaterial != materialUIDMap.end()) {
		return thisMaterial->second;
	}
	//Create a new material
	Material* newMaterial = new Material(_uniqueID);
	//Set the material's color
	newMaterial->SetColor(_color);
	//Add the material to the material map
	materialUIDMap[_uniqueID] = newMaterial;
	//Return new material
	return newMaterial;
}

Mesh* ResourceManager::CreateMeshFromData(Vertex _vertexArray[], UINT _vertexCount, int _indexArray[], UINT _indexCount, string _uniqueID) {
	//First, look for a mesh with this ID.  If it exists, return it
	auto thisMesh = meshUIDMap.find(_uniqueID);
	if (thisMesh != meshUIDMap.end()) {
		return thisMesh->second;
	}
	//If the mesh doesn't already exist, create it
	Mesh* newMesh = new Mesh(_vertexArray, _vertexCount, _indexArray, _indexCount, dxDevice);
	//Add it to the map
	meshUIDMap[_uniqueID] = newMesh;
	//return the new mesh
	return newMesh;
}

SimpleVertexShader* ResourceManager::GetVertexShader(LPCWSTR _filepath) {
	//If the shader already exists, return it
	auto thisShader = vertexShaderUIDMap.find(_filepath);
	if (thisShader != vertexShaderUIDMap.end()) {
		return thisShader->second;
	}
	//Create new shader
	SimpleVertexShader* newShader = new SimpleVertexShader(dxDevice, dxContext);
	newShader->LoadShaderFile(_filepath);
	//Store it in the shader map
	vertexShaderUIDMap[_filepath] = newShader;
	return newShader;
}

SimplePixelShader* ResourceManager::GetPixelShader(LPCWSTR _filepath) {
	//If the shader already exists, return it
	auto thisShader = pixelShaderUIDMap.find(_filepath);
	if (thisShader != pixelShaderUIDMap.end()) {
		return thisShader->second;
	}
	//Create new shader
	SimplePixelShader* newShader = new SimplePixelShader(dxDevice, dxContext);
	newShader->LoadShaderFile(_filepath);
	//Store it in the shader map
	pixelShaderUIDMap[_filepath] = newShader;
	return newShader;
}

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
	Release();
}

void ResourceManager::Release() {
	//Clean up and unload all resources
	//Clean up meshes
	map<string, Mesh*>::iterator meshIt;
	for (meshIt = meshUIDMap.begin(); meshIt != meshUIDMap.end(); ++meshIt) {
		Mesh* tempMesh = meshIt->second;
		if (tempMesh != nullptr) {
			delete tempMesh;
		}
	}
	meshUIDMap.clear();
	//Clean up vertex shaders
	map<LPCWSTR, SimpleVertexShader*>::iterator vsIt;
	for (vsIt = vertexShaderUIDMap.begin(); vsIt != vertexShaderUIDMap.end(); ++vsIt) {
		SimpleVertexShader* tempShader = vsIt->second;
		if (tempShader != nullptr) {
			delete tempShader;
		}
	}
	vertexShaderUIDMap.clear();
	//Clean up pixel shaders
	map<LPCWSTR, SimplePixelShader*>::iterator psIt;
	for (psIt = pixelShaderUIDMap.begin(); psIt != pixelShaderUIDMap.end(); ++psIt) {
		SimplePixelShader* tempShader = psIt->second;
		if (tempShader != nullptr) {
			delete tempShader;
		}
	}
	pixelShaderUIDMap.clear();
	//Clean up materials
	map<string, Material*>::iterator matIt;
	for (matIt = materialUIDMap.begin(); matIt != materialUIDMap.end(); ++matIt) {
		Material* tempMat = matIt->second;
		if (tempMat != nullptr) {
			delete tempMat;
		}
	}
	materialUIDMap.clear();
}
