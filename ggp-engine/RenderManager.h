#ifndef GGP_MESH_MANAGER_H
#define GGP_MESH_MANAGER_H

#include <map>
#include "SimpleShader.h"
#include "LightStructs.h"
#include <DirectXMath.h>
class Mesh;
class MeshRenderer;
class GameObject;
class ResourceManager;
class LightManager;
class ParticleManager;
struct ID3D11SamplerState;
class Camera;

typedef UINT MeshRendererID;

class RenderManager {
	//Singleton pointer
	static RenderManager* instance;

	ResourceManager* resourceManager;
	LightManager* lightManager;
	ParticleManager* particleManager;

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
	MeshRendererID mrCount = 0;
	//Map of all mesh renderers
	std::map<MeshRendererID, MeshRenderer*> meshRendererUIDMap;

	DirectX::XMFLOAT4 dofPara = DirectX::XMFLOAT4(2.0f, 15.0f, 85.0f, 15.0f);
	//x <-- near blur depth   
	//y <-- focal plane depth
	//z <-- far blur depth
	//w <-- blurriness cutoff constant for objects behind the focal plane

public:
	//Static Singleton get/release for the single renderManager instance
	static RenderManager* GetInstance();
	static void ReleaseInstance();

	// We don't want anything making copies of this class so delete these operators
	RenderManager(RenderManager const&) = delete;
	void operator=(RenderManager const&) = delete;

	//Start method.  Called once when we can safely assume the entire engine has been initialized
	void Start();

	//Called once per frame from the Game.cpp. Renders the active scene
	void Render();

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
	UINT AddMeshRenderer(MeshRenderer* _meshRenderer);
	//Get a mesh renderer given its unique identifier
	MeshRenderer* GetMeshRenderer(MeshRendererID _uniqueID);
	void RemoveMeshRenderer(MeshRenderer* _meshRenderer);

	/// <summary>
	/// Set the current skybox pixel shader
	/// </summary>
	/// <param name="aSkyPS">The new ksybox pixel shader</param>
	void SetSkyboxPS(SimplePixelShader* aSkyPS);

	/// <summary>
	/// Set the current skybox Vertex shader
	/// </summary>
	/// <param name="aSkyVS">The new skybox vertex shader</param>
	void SetSkyboxVS(SimpleVertexShader* aSkyVS);

private:
	RenderManager();
	~RenderManager();
	void Release();
};

#endif //GGP_MESH_MANAGER_H
