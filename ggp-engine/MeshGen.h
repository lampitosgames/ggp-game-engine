#ifndef GGP_MESH_GEN_H
#define GGP_MESH_GEN_H

#include "stdafx.h"

#include <vector>
#include "Mesh.h"
#include "Vertex.h"

class MeshGen {
	ID3D11Device* dxDevice;
	float PI = 3.14159264349f;

	struct vert {
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Vector2 uv;
		DirectX::SimpleMath::Vector3 norm;
		std::vector<UINT> faces;
	};
	struct tri {
		UINT vert1;
		UINT vert2;
		UINT vert3;
		DirectX::SimpleMath::Vector3 normal;
	};

	//Full mesh data storage
	UINT vIndex;
	std::vector<vert> vertData;
	UINT tIndex;
	std::vector<tri>  triData;

	//Mesh geometry builder functions
	void StartMesh();
	UINT addVert(DirectX::SimpleMath::Vector3 _pos, DirectX::SimpleMath::Vector2 _uv);
	UINT addTri(UINT _v1, UINT _v2, UINT _v3, bool _invertTanNormal = false);
	void addQuad(UINT _v1, UINT _v2, UINT _v3, UINT _v4, bool _invertTanNormal = false);

	//Get a triangle's normal vector given it's three vertices
	DirectX::SimpleMath::Vector3 getFaceNormal(DirectX::SimpleMath::Vector3 _pos1, DirectX::SimpleMath::Vector3 _pos2, DirectX::SimpleMath::Vector3 _pos3);

	//Build the mesh with individual verts for each face, with unique normals and uv coords
	void buildWithFaceNormals();
	//Build the mesh with shared verts that have an average normal
	void buildWithSmoothNormals();

	//Calculate tangent vectors for each vertex in a mesh
	void CalculateTangents();

	//Post-build storage for the mesh data
	UINT vertCount;
	std::vector<Vertex> mVerts;
	UINT indCount;
	std::vector<UINT> mIndices;
public:
	MeshGen();
	MeshGen(ID3D11Device* _dxDevice);

	Mesh* LoadTerrain(ResName _rawFilepath, UINT _resolution, float _heightScale = 50.0f, float _uvScale = 30.0f);

	Mesh* GenerateCube(float _sideLength, float _uvScale = 1.0f);
	Mesh* GenerateSphere(float _radius, UINT _subdivs = 4, float _uvScale = 1.0f);
};

#endif //GGP_MESH_GEN_H