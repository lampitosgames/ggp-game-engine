#include "stdafx.h"

#include "MeshGen.h"

using namespace DirectX;

void MeshGen::StartMesh() {
	vIndex = -1;
	tIndex = -1;
	vertCount = 0;
	vertData.clear();
	triData.clear();
	mVerts.clear();
	mIndices.clear();
}

#pragma region Adding Geometry
UINT MeshGen::addVert(float3 _pos, float2 _uv) {
	//Add a new vert struct to the vertData array
	vertData.push_back({
		_pos,
		_uv,
		float3()
	});
	//Increment and return the index
	vIndex++;
	return vIndex;
}

/*
	v3 - v2
	|   /
	|  /
	v1
*/
UINT MeshGen::addTri(UINT _v1, UINT _v2, UINT _v3) {
	//Get normal vector to the plane
	float3 triNormal = getFaceNormal(vertData[_v1].pos, vertData[_v2].pos, vertData[_v3].pos);
	//Push a new tri face onto the triData array
	triData.push_back({
		_v1,
		_v2,
		_v3,
		triNormal
	});
	//Increment the tri index
	tIndex++;
	//Add this tri to the verts that make it up
	vertData[_v1].faces.push_back(tIndex);
	vertData[_v2].faces.push_back(tIndex);
	vertData[_v3].faces.push_back(tIndex);
	//Return the index of this face
	return tIndex;
}

/*
	v4 -- v3
	|      |
	|      |
	v1 -- v2
*/
void MeshGen::addQuad(UINT _v1, UINT _v2, UINT _v3, UINT _v4) {
	addTri(_v1, _v3, _v4);
	addTri(_v1, _v2, _v3);
}
#pragma endregion

float3 MeshGen::getFaceNormal(float3 _pos1, float3 _pos2, float3 _pos3) {
	//Get the face's bounding vectors
	float3 vec1 = float3(_pos2.x - _pos1.x, _pos2.y - _pos1.y, _pos2.z - _pos1.z);
	float3 vec2 = float3(_pos3.x - _pos1.x, _pos3.y - _pos1.y, _pos3.z - _pos1.z);
	//Get normalized cross product
	XMVECTOR cross = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&vec1), XMLoadFloat3(&vec2)));
	//Store and return
	float3 storedCross;
	XMStoreFloat3(&storedCross, cross);
	return storedCross;
}

void MeshGen::buildWithFaceNormals() {
	//Loop through every face
	for (UINT i = 0; i <= tIndex; i++) {
		tri curTri = triData[i];
		//Push the three verts onto the output array
		mVerts.push_back({vertData[curTri.vert1].pos, vertData[curTri.vert1].uv, curTri.normal});
		mVerts.push_back({vertData[curTri.vert2].pos, vertData[curTri.vert2].uv, curTri.normal});
		mVerts.push_back({vertData[curTri.vert3].pos, vertData[curTri.vert3].uv, curTri.normal});
		//Push their indices onto the output index array
		mIndices.push_back(vertCount++);
		mIndices.push_back(vertCount++);
		mIndices.push_back(vertCount++);
	}
}

void MeshGen::buildWithSmoothNormals() {
	//Loop through all verts and calculate normals from faces.  Create Vertex struct and add it to the final array
	//Loop through all tris and simply pass the stored index into the index buffer
}

// Calculates the tangents of the vertices in a mesh
// Code adapted from: http://www.terathon.com/code/tangent.html
// Taken from the demo on mycourses (Game Graphics Programming)
void MeshGen::CalculateTangents() {
	// Reset tangents
	for (UINT i = 0; i < vertCount; i++) {
		mVerts[i].tangent = float3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (UINT i = 0; i < vertCount;) {
		// Grab indices and vertices of first triangle
		unsigned int i1 = mIndices[i++];
		unsigned int i2 = mIndices[i++];
		unsigned int i3 = mIndices[i++];
		Vertex* v1 = &mVerts[i1];
		Vertex* v2 = &mVerts[i2];
		Vertex* v3 = &mVerts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->position.x - v1->position.x;
		float y1 = v2->position.y - v1->position.y;
		float z1 = v2->position.z - v1->position.z;

		float x2 = v3->position.x - v1->position.x;
		float y2 = v3->position.y - v1->position.y;
		float z2 = v3->position.z - v1->position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->uv.x - v1->uv.x;
		float t1 = v2->uv.y - v1->uv.y;

		float s2 = v3->uv.x - v1->uv.x;
		float t2 = v3->uv.y - v1->uv.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->tangent.x += tx;
		v1->tangent.y += ty;
		v1->tangent.z += tz;

		v2->tangent.x += tx;
		v2->tangent.y += ty;
		v2->tangent.z += tz;

		v3->tangent.x += tx;
		v3->tangent.y += ty;
		v3->tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (UINT i = 0; i < vertCount; i++) {
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&mVerts[i].normal);
		XMVECTOR tangent = XMLoadFloat3(&mVerts[i].tangent);

		// Use Gram-Schmidt orthogonalize
		tangent = XMVector3Normalize(tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&mVerts[i].tangent, tangent);
	}
}

MeshGen::MeshGen() { dxDevice = nullptr; }

MeshGen::MeshGen(ID3D11Device* _dxDevice) {
	dxDevice = _dxDevice;
}

Mesh* MeshGen::GenerateCube(float _sideLength, float _uvScale) {
	StartMesh();
	//Half length
	float sl = _sideLength * 0.5f;
	//Create all 8 positions
	float3 fnw(-sl, +sl, +sl); //front north west
	float3 fne(+sl, +sl, +sl); //front north east
	float3 fsw(-sl, -sl, +sl); //front south west
	float3 fse(+sl, -sl, +sl); //front south east
	float3 bnw(-sl, +sl, -sl); //back north west
	float3 bne(+sl, +sl, -sl); //back north east
	float3 bsw(-sl, -sl, -sl); //back south west
	float3 bse(+sl, -sl, -sl); //back south east

	//Vert index storage
	UINT v1, v2, v3, v4;

	//Add all faces. Remember that winding order is counter-clockwise
	//Front face
	v1 = addVert(fsw, float2(0.0f, _uvScale));
	v2 = addVert(fse, float2(_uvScale, _uvScale));
	v3 = addVert(fne, float2(_uvScale, 0.0f));
	v4 = addVert(fnw, float2(0.0f, 0.0f));
	addQuad(v1, v2, v3, v4);
	//Right face
	v1 = addVert(fse, float2(0.0f, _uvScale));
	v2 = addVert(bse, float2(_uvScale, _uvScale));
	v3 = addVert(bne, float2(_uvScale, 0.0f));
	v4 = addVert(fne, float2(0.0f, 0.0f));
	addQuad(v1, v2, v3, v4);
	//Back face
	v1 = addVert(bse, float2(0.0f, _uvScale));
	v2 = addVert(bsw, float2(_uvScale, _uvScale));
	v3 = addVert(bnw, float2(_uvScale, 0.0f));
	v4 = addVert(bne, float2(0.0f, 0.0f));
	addQuad(v1, v2, v3, v4);
	//Left face
	v1 = addVert(bsw, float2(0.0f, _uvScale));
	v2 = addVert(fsw, float2(_uvScale, _uvScale));
	v3 = addVert(fnw, float2(_uvScale, 0.0f));
	v4 = addVert(bnw, float2(0.0f, 0.0f));
	addQuad(v1, v2, v3, v4);
	//Top face
	v1 = addVert(fnw, float2(0.0f, _uvScale));
	v2 = addVert(fne, float2(_uvScale, _uvScale));
	v3 = addVert(bne, float2(_uvScale, 0.0f));
	v4 = addVert(bnw, float2(0.0f, 0.0f));
	addQuad(v1, v2, v3, v4);
	//Bottom face
	v1 = addVert(bsw, float2(0.0f, _uvScale));
	v2 = addVert(bse, float2(_uvScale, _uvScale));
	v3 = addVert(fse, float2(_uvScale, 0.0f));
	v4 = addVert(fsw, float2(0.0f, 0.0f));
	UINT tri1 = addTri(v1, v2, v3);
	UINT tri2 = addTri(v1, v3, v4);

	//Build the mesh without shared verts
	buildWithFaceNormals();
	//Calculate tangents
	CalculateTangents();

	//Create and return the new cube mesh
	Mesh* newMesh = new Mesh(&mVerts[0], vertCount, &mIndices[0], vertCount, dxDevice);
	return newMesh;
}
