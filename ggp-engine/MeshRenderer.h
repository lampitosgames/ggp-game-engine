#ifndef GGP_MESH_RENDERER_H
#define GGP_MESH_RENDERER_H

//This class is a wrapper for the mesh class that should handle loading/generating

#include <DirectXMath.h>
#include <d3d11.h>
#include "Component.h"
class Mesh;
class Material;
class GameObject;
class SimpleVertexShader;
class SimplePixelShader;

class MeshRenderer : public Component {
	Mesh* mesh;
	Material* material;
public:
    //GameObject * gameObject;
	MeshRenderer(UINT _uniqueID, GameObject* _gameObject);
	MeshRenderer(UINT _uniqueID, GameObject* _gameObject, Mesh* _mesh, Material* _material);
	~MeshRenderer();

	void SetMesh(Mesh* _mesh);
	void SetMaterial(Material* _material);
	void Draw(ID3D11DeviceContext* _dxContext);

	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInvTransMatrix();
	Mesh* GetMesh();
	Material* GetMaterial();
	//Quick access to the material's members
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	DirectX::XMFLOAT4 GetColor();
};

#endif //GGP_MESH_RENDERER_H