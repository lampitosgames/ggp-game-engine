#include "ResourceManager.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Mesh.h"
#include <fstream>
#include <iostream>

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

#pragma region Material Creation/Loading
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
#pragma endregion

#pragma region Shader Loading
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
#pragma endregion

#pragma region Mesh Loading/Generation
Mesh* ResourceManager::CreateMeshFromData(Vertex* _vertexArray, UINT _vertexCount, UINT* _indexArray, UINT _indexCount, string _uniqueID) {
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

Mesh* ResourceManager::GetMesh(string _uniqueID) {
	//If the mesh already exists, return it
	auto thisMesh = meshUIDMap.find(_uniqueID);
	if (thisMesh != meshUIDMap.end()) {
		return thisMesh->second;
	}
	//Else, we have to load the mesh from the filepath
	Mesh* newMesh = LoadMesh(_uniqueID);
	//Null check.  All kinds of weird things can happen with file I/O
	if (newMesh == nullptr) {
		return nullptr;
	}
	//Store it in the mesh map
	meshUIDMap[_uniqueID] = newMesh;
	return newMesh;
}

Mesh* ResourceManager::LoadMesh(string _filepath) {// File input object
	std::ifstream obj(_filepath);

	// Check for successful open
	if (!obj.is_open()) {
		return nullptr;
	}

	// Variables used while reading the file
	unsigned int vertCounter = 0;        // Count of vertices/indices
	char chars[100];

	// Still have data left?
	while (obj.good()) {
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n') {
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(chars, "vn %f %f %f", &norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			mNormals.push_back(norm);
		} else if (chars[0] == 'v' && chars[1] == 't') {
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(chars, "vt %f %f", &uv.x, &uv.y);

			// Add to the list of uv's
			mUVs.push_back(uv);
		} else if (chars[0] == 'v') {
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(chars, "v %f %f %f", &pos.x, &pos.y, &pos.z);

			// Add to the positions
			mPositions.push_back(pos);
		} else if (chars[0] == 'f') {
			// Read the face indices into an array
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.position = mPositions[i[0] - 1];
			v1.uv = mUVs[i[1] - 1];
			v1.normal = mNormals[i[2] - 1];

			Vertex v2;
			v2.position = mPositions[i[3] - 1];
			v2.uv = mUVs[i[4] - 1];
			v2.normal = mNormals[i[5] - 1];

			Vertex v3;
			v3.position = mPositions[i[6] - 1];
			v3.uv = mUVs[i[7] - 1];
			v3.normal = mNormals[i[8] - 1];

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.uv.y = 1.0f - v1.uv.y;
			v2.uv.y = 1.0f - v2.uv.y;
			v3.uv.y = 1.0f - v3.uv.y;

			// Flip Z (LH vs. RH)
			v1.position.z *= -1.0f;
			v2.position.z *= -1.0f;
			v3.position.z *= -1.0f;

			// Flip normal Z
			v1.normal.z *= -1.0f;
			v2.normal.z *= -1.0f;
			v3.normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			mVerts.push_back(v1);
			mVerts.push_back(v3);
			mVerts.push_back(v2);

			// Add three more indices
			mIndices.push_back(vertCounter); vertCounter += 1;
			mIndices.push_back(vertCounter); vertCounter += 1;
			mIndices.push_back(vertCounter); vertCounter += 1;

			// Was there a 4th face?
			if (facesRead == 12) {
				// Make the last vertex
				Vertex v4;
				v4.position = mPositions[i[9] - 1];
				v4.uv = mUVs[i[10] - 1];
				v4.normal = mNormals[i[11] - 1];

				// Flip the UV, Z pos and normal
				v4.uv.y = 1.0f - v4.uv.y;
				v4.position.z *= -1.0f;
				v4.normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				mVerts.push_back(v1);
				mVerts.push_back(v4);
				mVerts.push_back(v3);

				// Add three more indices
				mIndices.push_back(vertCounter); vertCounter += 1;
				mIndices.push_back(vertCounter); vertCounter += 1;
				mIndices.push_back(vertCounter); vertCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();

	Mesh* newMesh = new Mesh(&mVerts[0], vertCounter, &mIndices[0], vertCounter, dxDevice);
	//Clean up structs
	mPositions.clear();
	mNormals.clear();
	mUVs.clear();
	mVerts.clear();
	mIndices.clear();
	//Return the newly created mesh
	return newMesh;
}
#pragma endregion

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
