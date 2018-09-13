#ifndef GGP_MESH_MANAGER_H
#define GGP_MESH_MANAGER_H

#include <map>
#include "SimpleShader.h"
class Mesh;
class MeshRenderer;
class Spatial;

class MeshManager {
	//Singleton pointer
	static MeshManager* instance;

	//Unique number given to each mesh renderer
	UINT mrUID = 0;
	//Map of all mesh renderers
	std::map<UINT, MeshRenderer*> meshRendererUIDMap;

	//Map of all meshes
	std::map<std::string, Mesh*> meshUIDMap;
public:
	//Static Singleton get/release for the single MeshManager instance
	static MeshManager* GetInstance();
	static void ReleaseInstance();

	//TODO: Refactor so this requires fewer arguments. Grab the data inside the render function (like a camera object or the material component of each object)
	void Render(ID3D11DeviceContext* _dxContext, SimpleVertexShader* _vertexShader, SimplePixelShader* pixelShader, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);

	/*
		MESH RENDERER HELPERS
		Useful for sharing one mesh asset across many objects
	*/
	//Create (and return the uid of) a new mesh renderer
	UINT AddMeshRenderer(Spatial* _gameObject);
	//Get a mesh renderer given its unique identifier
	MeshRenderer* GetMeshRenderer(UINT _uniqueID);
	//Delete a mesh renderer
	void DeleteMeshRenderer(UINT _uniqueID);

	/*
		MESH HELPERS
		TODO: primitive generation. Once generated, store the result and reuse as needed
		TODO: Mesh loading from files
		TODO: Automatically detect if a requested mesh is already loaded and reuse the same asset
	*/
	//Mesh* LoadMesh(std::string _filepath, std::string _uniqueID);
	//Mesh* GetMesh(std::string _uniqueID);
	//void DeleteMesh(std::string _uniqueID);
private:
	MeshManager();
	~MeshManager();
	void Release();
};

#endif //GGP_MESH_MANAGER_H
