#ifndef GGP_MESH_RENDERER_H
#define GGP_MESH_RENDERER_H

//This class is a wrapper for the mesh class that should handle loading/generating

#include <DirectXMath.h>
#include <d3d11.h>
#include "Component.h"
class Mesh;
class Spatial;

class MeshRenderer : public Component {
	Mesh* mesh;
public:
	Spatial* gameObject;
	MeshRenderer(UINT _uniqueID, Spatial* _gameObject);
	~MeshRenderer();

	void SetMesh(Mesh* _mesh);
	void Draw(ID3D11DeviceContext* _dxContext);

	DirectX::XMFLOAT4X4 GetWorldMatrix();
	Mesh* GetMesh();
};

#endif //GGP_MESH_RENDERER_H