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
	//List of all mesh renderers
	std::map<UINT, MeshRenderer*> meshRendererMap;
public:
	//Static Singleton get/release for the single MeshManager instance
	static MeshManager* GetInstance();
	static void ReleaseInstance();

	//Create (and return the uid of) a new mesh renderer
	UINT AddMeshRenderer(Spatial* _gameObject);
	//Get a mesh renderer given its unique identifier
	MeshRenderer* GetMeshRenderer(UINT _uniqueID);
	//Delete a mesh renderer
	void DeleteMeshRenderer(UINT _uniqueID);

	//TODO: Refactor so this requires fewer arguments. Grab the data inside the render function (like a camera object or the material component of each object)
	void Render(ID3D11DeviceContext* _dxContext, SimpleVertexShader* _vertexShader, SimplePixelShader* pixelShader, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);
private:
	MeshManager();
	~MeshManager();
	void Release();
};

#endif //GGP_MESH_MANAGER_H
