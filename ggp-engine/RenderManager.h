#ifndef GGP_MESH_MANAGER_H
#define GGP_MESH_MANAGER_H

#include <map>
#include "SimpleShader.h"
#include "LightStructs.h"
class Mesh;
class MeshRenderer;
class GameObject;
class ResourceManager;
class LightManager;
struct ID3D11SamplerState;
class Camera;

class RenderManager {
	//Singleton pointer
	static RenderManager* instance;

	ResourceManager* resourceManager;
	LightManager* lightManager;

	//Texture sampler used for all textures currently (Add more later for different settings)
	ID3D11SamplerState* samplerState;

	//Default shaders for materials without them (Or MeshRenderers without materials)
	SimpleVertexShader* defaultVertexShader;
	SimplePixelShader* defaultPixelShader;
    
    // Skybox options
    SimpleVertexShader* skyboxVS = nullptr;
    SimplePixelShader* skyboxPS = nullptr;
    ID3D11RasterizerState* skyRastState = nullptr;
    ID3D11DepthStencilState* skyDepthState = nullptr;
    ID3D11ShaderResourceView* skyboxSrv = nullptr;
    Mesh* skyboxMesh = nullptr;

	//Active camera
	Camera* activeCamera;

	//Gamma correction value
	float gammaCorrection;

	//Unique number given to each mesh renderer
	UINT mrUID = 0;
	//Map of all mesh renderers
	std::map<UINT, MeshRenderer*> meshRendererUIDMap;
public:
	//Static Singleton get/release for the single renderManager instance
	static RenderManager* GetInstance();
	static void ReleaseInstance();

    // We don't want anything making copies of this class so delete these operators
    RenderManager( RenderManager const& ) = delete;
    void operator=( RenderManager const& ) = delete;

	//Start method.  Called once when we can safely assume the entire engine has been initialized
	void Start();

	//Called once per frame from the Game.cpp. Renders the active scene
	void Render(ID3D11DeviceContext* _dxContext);

	//Gamma correction get/set
	float GetGammaCorrection();
	void SetGammaCorrection(float _newGamma);

	//Active camera get/set
	Camera* GetActiveCamera();
	void SetActiveCamera(Camera* _newCamera);

	/*
		MESH RENDERER HELPERS
		Useful for sharing one mesh asset across many objects
	*/
	//Create (and return the uid of) a new mesh renderer
	UINT AddMeshRenderer( GameObject* _gameObject);
    
    UINT AddMeshRenderer( MeshRenderer* _meshRenderer );

	//Get a mesh renderer given its unique identifier
	MeshRenderer* GetMeshRenderer(UINT _uniqueID);
	//Delete a mesh renderer
	void DeleteMeshRenderer(UINT _uniqueID);

    /// <summary>
    /// Set the current skybox pixel shader
    /// </summary>
    /// <param name="aSkyPS">The new ksybox pixel shader</param>
    void SetSkyboxPS( SimplePixelShader* aSkyPS );

    /// <summary>
    /// Set the current skybox Vertex shader
    /// </summary>
    /// <param name="aSkyVS">The new skybox vertex shader</param>
    void SetSkyboxVS( SimpleVertexShader* aSkyVS );

private:
	RenderManager();
	~RenderManager();
	void Release();
};

#endif //GGP_MESH_MANAGER_H
