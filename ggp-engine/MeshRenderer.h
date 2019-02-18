#ifndef GGP_MESH_RENDERER_H
#define GGP_MESH_RENDERER_H

//This class is a wrapper for the mesh class that should handle loading/generating

#include <d3d11.h>
#include <SimpleMath.h>
#include "BaseComponent.h"

class Mesh;
class Material;
class GameObject;
class SimpleVertexShader;
class SimplePixelShader;
class RenderManager;

class MeshRenderer : public BaseComponent<MeshRenderer> {
	Mesh* mesh;
	Material* material;
public:
	MeshRenderer(GameObject* _gameObject);
	MeshRenderer(GameObject* _gameObject, Mesh* _mesh, Material* _material);
	~MeshRenderer();

	void SetMesh(Mesh* _mesh);
	void SetMaterial(Material* _material);
	void Draw(ID3D11DeviceContext* _dxContext);

	DirectX::SimpleMath::Matrix GetWorldMatrix();
	DirectX::SimpleMath::Matrix GetWorldInvTransMatrix();
	Mesh* GetMesh();
	Material* GetMaterial();
	//Quick access to the material's members
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	DirectX::SimpleMath::Color GetColor();

	GameObject* gameObject;
};

#endif //GGP_MESH_RENDERER_H