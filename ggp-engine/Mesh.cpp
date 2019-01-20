#include "stdafx.h"

#include "Mesh.h"

//Mesh constructor
Mesh::Mesh(Vertex* _vertexArray,
		   UINT _vertexCount,
		   UINT* _indexArray,
		   UINT _indexCount,
		   ID3D11Device * _dxDevice) {
	//Store the index count locally
	indexCount = _indexCount;

	//Create a vertex buffer description
	D3D11_BUFFER_DESC meshVBD;
	meshVBD.Usage = D3D11_USAGE_IMMUTABLE;
	meshVBD.ByteWidth = sizeof(Vertex) * _vertexCount;
	meshVBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	meshVBD.CPUAccessFlags = 0;
	meshVBD.MiscFlags = 0;
	meshVBD.StructureByteStride = 0;
	//Create an index buffer description
	D3D11_BUFFER_DESC meshIBD;
	meshIBD.Usage = D3D11_USAGE_IMMUTABLE;
	meshIBD.ByteWidth = sizeof(int) * _indexCount;
	meshIBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	meshIBD.CPUAccessFlags = 0;
	meshIBD.MiscFlags = 0;
	meshIBD.StructureByteStride = 0;

	//Struct to hold initial vertex data that we'll put in the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = _vertexArray;
	//Second struct to hold initial index data
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = _indexArray;

	//Create the buffers and store them in member variables
	_dxDevice->CreateBuffer(&meshVBD, &initialVertexData, &vertexBuffer);
	_dxDevice->CreateBuffer(&meshIBD, &initialIndexData, &indexBuffer);
}

Mesh::~Mesh(void) {
	vertexBuffer->Release();
	indexBuffer->Release();
}

ID3D11Buffer* Mesh::GetVertexBuffer() {
	return vertexBuffer;
}

ID3D11Buffer* Mesh::GetIndexBuffer() {
	return indexBuffer;
}

UINT Mesh::GetIndexCount() {
	return indexCount;
}

UINT Mesh::GetStride() {
	return stride;
}

UINT Mesh::GetOffset() {
	return offset;
}
