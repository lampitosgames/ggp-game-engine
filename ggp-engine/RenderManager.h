#ifndef GGP_MESH_MANAGER_H
#define GGP_MESH_MANAGER_H

#include <map>
#include "SimpleShader.h"
#include "Light.h"
class Mesh;
class MeshRenderer;
class Spatial;
class ResourceManager;

class RenderManager {
	//Singleton pointer
	static RenderManager* instance;

	ResourceManager* resourceManager;

	//Default shaders for materials without them (Or MeshRenderers without materials)
	SimpleVertexShader* defaultVertexShader;
	SimplePixelShader* defaultPixelShader;

	//Unique number given to each mesh renderer
	UINT mrUID = 0;
	//Map of all mesh renderers
	std::map<UINT, MeshRenderer*> meshRendererUIDMap;

	//TEMP: Lights until they are moved into game objects
	DirectionalLight lightArray[9];
	UINT lightCount;
public:
	//Static Singleton get/release for the single renderManager instance
	static RenderManager* GetInstance();
	static void ReleaseInstance();

	//Start method.  Called once when we can safely assume the entire engine has been initialized
	void Start();

	//TODO: Refactor so this requires fewer arguments. Grab the data inside the render function (like a camera object or the material component of each object)
	void Render(ID3D11DeviceContext* _dxContext, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);

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

	//TODO: Mesh primitive generation.  Works together with the resourceManager
private:
	RenderManager();
	~RenderManager();
	void Release();
};

#endif //GGP_MESH_MANAGER_H