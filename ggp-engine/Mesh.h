#ifndef GGP_MESH_H
#define GGP_MESH_H

#include "Vertex.h"
#include <d3d11.h>

//Created by Daniel Timko - Assignment 2

class Mesh {
	//DirectX Buffers for this mesh
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	//Index count of this mesh
	UINT indexCount;
public:
	//Constructor
	Mesh(Vertex vertexArray[], UINT vertexCount, int vertexIndicesArray[], UINT indexCount, ID3D11Device* dxDevice);
	//Destructor
	~Mesh(void);

	//Getters
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	UINT GetIndexCount();
};

#endif //GGP_MESH_H