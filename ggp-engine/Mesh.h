#ifndef GGP_MESH_H
#define GGP_MESH_H

#include <d3d11.h>
#include "Vertex.h"

//Created by Daniel Timko - Assignment 2

class Mesh {
	//DirectX Buffers for this mesh
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	//Index count of this mesh
	UINT indexCount;
	//Stride and offset of the buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//Does this mesh cast shadows?
	bool castsShadows = false;
public:
	//Constructor
	Mesh(Vertex* vertexArray, UINT vertexCount, UINT* vertexIndicesArray, UINT indexCount, ID3D11Device* dxDevice);
	//Destructor
	~Mesh(void);

	//Getters
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	UINT GetIndexCount();
	UINT GetStride();
	UINT GetOffset();
	bool GetCastShadows();

	//Setters
	void SetCastShadows(bool _castsShadows);
};

#endif //GGP_MESH_H