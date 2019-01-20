#include "stdafx.h"

#include "MeshGen.h"
#include <map>
#include <fstream>
#include <iostream>

using namespace DirectX;

void MeshGen::StartMesh() {
	vIndex = -1;
	tIndex = -1;
	vertCount = 0;
	indCount = 0;
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
UINT MeshGen::addTri(UINT _v1, UINT _v2, UINT _v3, bool _invertTanNormal) {
	//Get normal vector to the plane
	float3 triNormal = getFaceNormal(vertData[_v1].pos, vertData[_v2].pos, vertData[_v3].pos);
	//Invert the normal? Used when winding order and the tangent calculation don't line up exactly right
	triNormal = _invertTanNormal ? float3(-triNormal.x, -triNormal.y, -triNormal.z) : triNormal;
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
void MeshGen::addQuad(UINT _v1, UINT _v2, UINT _v3, UINT _v4, bool _invertTanNormal) {
	addTri(_v1, _v3, _v4, _invertTanNormal);
	addTri(_v1, _v2, _v3, _invertTanNormal);
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
		mVerts.push_back({vertData[curTri.vert1].pos, vertData[curTri.vert1].uv, curTri.normal}); vertCount++;
		mVerts.push_back({vertData[curTri.vert2].pos, vertData[curTri.vert2].uv, curTri.normal}); vertCount++;
		mVerts.push_back({vertData[curTri.vert3].pos, vertData[curTri.vert3].uv, curTri.normal}); vertCount++;
		//Push their indices onto the output index array
		mIndices.push_back(indCount++);
		mIndices.push_back(indCount++);
		mIndices.push_back(indCount++);
	}
}

void MeshGen::buildWithSmoothNormals() {
	//Loop through all verts and calculate normals from faces.  Create Vertex struct and add it to the final array
	for (UINT i = 0; i <= vIndex; i++) {
		vert curVert = vertData[i];
		XMVECTOR averageNormal = XMVectorZero();
		//Loop through all faces this vert belongs to and average them
		for (UINT f = 0; f < curVert.faces.size(); f++) {
			averageNormal += XMLoadFloat3(&triData[curVert.faces[f]].normal);
		}
		averageNormal = XMVector3Normalize(averageNormal);
		//Store the normal
		XMStoreFloat3(&curVert.norm, averageNormal);
		//Add the vertex and it's averaged normal to the output array
		mVerts.push_back({curVert.pos, curVert.uv, curVert.norm});
		vertCount++;
	}
	//Loop through all tris and simply pass the stored index into the index buffer
	for (UINT t = 0; t <= tIndex; t++) {
		tri curTri = triData[t];
		mIndices.push_back(curTri.vert1); indCount++;
		mIndices.push_back(curTri.vert2); indCount++;
		mIndices.push_back(curTri.vert3); indCount++;
	}
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
	for (UINT i = 0; i < indCount;) {
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

Mesh* MeshGen::LoadTerrain(std::string _rawFilepath, UINT _resolution, float _heightScale, float _uvScale) {
	StartMesh();
	unsigned char* heights = new unsigned char[_resolution * _resolution];
	std::ifstream file;
	file.open(_rawFilepath, std::ios_base::binary);
	if (file) {
		file.read((char*)&heights[0], _resolution * _resolution);
		file.close();
	} else {
		return nullptr;
	}

	//Scale. 1 pixel = this many world units
	float scl = 0.15f;

	UINT* vertInds = new UINT[_resolution * _resolution];
	for (UINT j = 0; j < _resolution * _resolution; j++) {
		//Get 2d coordinates from the 1d array
		int lx = j % _resolution;
		int lz = (int)floor(j / _resolution);

		//Average surrounding heights
		float avgHeight = 0.0f;
		UINT neighborInds[] = {
			j - _resolution - 1, //NW
			j - _resolution,     //N
			j - _resolution + 1, //NE
			j - 1,               //MW
			j,                   //M
			j + 1,               //ME
			j + _resolution - 1, //SW
			j + _resolution,     //S
			j + _resolution + 1  //SE
		};
		//If on first row, there is no previous row.
		if (j < _resolution) { neighborInds[0] = neighborInds[1] = neighborInds[2] = j; }
		//If on last row, there is no next row
		if (j > _resolution * (_resolution - 1)) { neighborInds[6] = neighborInds[7] = neighborInds[8] = j; }
		//If on first column, there is no previous column
		if (j % _resolution == 0) { neighborInds[0] = neighborInds[3] = neighborInds[6] = j; }
		//If on last column, there is no next column
		if (j % _resolution == _resolution - 1) { neighborInds[2] = neighborInds[5] = neighborInds[8] = j; }

		for (size_t h = 0; h < 9; h++) {
			avgHeight += heights[neighborInds[h]];
		}

		avgHeight /= 9.0f;

		//Get xyz coordinates
		float vx = ((float)lx * scl) - ((float)_resolution * scl / 2.0f);
		float vy = (avgHeight / 255.0f) * _heightScale; //Scale the raw height data to world coorda
		float vz = ((float)lz * scl) - ((float)_resolution * scl / 2.0f);

		//Get UV coordinates
		float u = _uvScale * ((float)lx / (float)_resolution);
		float v = _uvScale * ((float)lz / (float)_resolution);

		//Create a new vertex and store it in the vertInds array
		vertInds[j] = addVert(float3(vx, vy, vz), float2(u, v));
	}

	//Create faces for the terrain
	for (UINT k = 0; k < _resolution * (_resolution - 1) - 1; k++) {
		//Skip the last vertex in a row since it's already been included in the mesh
		if (k % _resolution == _resolution - 1) {
			continue;
		}
		//Get the 4 coorners of the quad
		UINT v1 = vertInds[k + _resolution];
		UINT v2 = vertInds[k + _resolution + 1];
		UINT v3 = vertInds[k + 1];
		UINT v4 = vertInds[k];
		addQuad(v1, v2, v3, v4);
	}

	//Build the mesh with shared verts and smoothed normals
	buildWithSmoothNormals();
	//Calculate tangents
	CalculateTangents();

	//Create and return the new cube mesh
	Mesh* newMesh = new Mesh(&mVerts[0], vertCount, &mIndices[0], indCount, dxDevice);
	delete[] heights;
	delete[] vertInds;
	return newMesh;
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
	Mesh* newMesh = new Mesh(&mVerts[0], vertCount, &mIndices[0], indCount, dxDevice);
	return newMesh;
}

//Code adapted from homework I did for DSA2
Mesh* MeshGen::GenerateSphere(float _radius, UINT _subdivs, float _uvScale) {
	//TODO: UVs are fucky and definitely need better math than I have time for
	StartMesh();
	//Set min/max for subdivisions
	_subdivs = max(1, min(6, _subdivs));

	//Initialize point data with a constant icosahedron to be further subdivided.
	//https://schneide.wordpress.com/2016/07/15/generating-an-icosphere-in-c/
	float x = sin(0.5532694f) * _radius;
	float z = cos(0.5532694f) * _radius;
	float n = 0.0f;
	//Vector of points that make up the sphere
	std::vector<float3> sphereVerts = {
		float3(-x, +n, +z), float3(+x, +n, +z), float3(-x, +n, -z), float3(+x, +n, -z),
		float3(+n, +z, +x), float3(+n, +z, -x), float3(+n, -z, +x), float3(+n, -z, -x),
		float3(+z, +x, +n), float3(-z, +x, +n), float3(+z, -x, +n), float3(-z, -x, +n)
	};
	//Vector of tri indices on the sphere
	std::vector<std::vector<int>> sphereTris = {
		{0,4,1},  {0,9,4},  {9,5,4},  {4,5,8},  {4,8,1},
		{8,10,1}, {8,3,10}, {5,3,8},  {5,2,3},  {2,7,3},
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
		{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5},  {7,2,11}
	};

	//For each requested subdivision, subdivide the mesh to make it a more accurate circle
	for (UINT s = 0; s < _subdivs; s++) {
		//Additional triangle storage
		std::vector<std::vector<int>> newTris;
		//Map that stores the index of a post-subdivide middle vertex given the two verts it is in the middle of
		std::map<std::pair<int, int>, int> edgesMiddleInd;

		//Loop through the triangles and subdivide each
		for (UINT t = 0; t < sphereTris.size(); t++) {
			//Store old tri
			std::vector<int> oldTri = sphereTris[t];
			//Store old corners
			int corner1 = oldTri[0];
			int corner2 = oldTri[1];
			int corner3 = oldTri[2];
			//Store pairs of corners as lookup values (sides) for the map of middle points
			std::pair<int, int> sides[3];
			sides[0] = std::pair<int, int>(corner1, corner2);
			sides[1] = std::pair<int, int>(corner2, corner3);
			sides[2] = std::pair<int, int>(corner3, corner1);

			for (int j = 0; j < 3; j++) {
				//Order the current sides pair from smallest to largest. This prevents 0,1 and 1,0 from 
				//being two different values in the map(which would generate 2 different middle vertices)
				if (sides[j].first > sides[j].second) { std::swap(sides[j].first, sides[j].second); }
				//If the middle vert for this side doesn't exist, create it
				auto middleInsert = edgesMiddleInd.insert({sides[j], (int)sphereVerts.size()});
				if (middleInsert.second) {
					//Get the average position between the two bounding verts and store it as an XMVECTOR
					XMVECTOR newVertVec = XMLoadFloat3(&sphereVerts[sides[j].first]) + XMLoadFloat3(&sphereVerts[sides[j].second]);
					//Generate the middle vertex and add it to the array.
					//radius * norm(point1 + point2) = a point equidistant from both point1 and point2, normalized onto the sphere's shell
					float3 newVert;
					XMStoreFloat3(&newVert, _radius * XMVector3Normalize(newVertVec));
					sphereVerts.push_back(newVert);
				}
			}

			//Get the array index (in the vert array) of each middle point
			int middle1 = edgesMiddleInd[sides[0]];
			int middle2 = edgesMiddleInd[sides[1]];
			int middle3 = edgesMiddleInd[sides[2]];
			//Push all 4 new tris into the new triangles array
			/*	  *
				 / \
				/ 4 \
			   *-----*
			  / \ 3 / \
			 / 1 \ / 2 \
			*-----*-----*/
			newTris.push_back({corner1,middle1,middle3}); //1
			newTris.push_back({middle1,corner2,middle2}); //2
			newTris.push_back({middle1,middle2,middle3}); //3
			newTris.push_back({middle3,middle2,corner3}); //4
		}
		//Replace the old triangles array with the new one for the next iteration
		sphereTris = newTris;
	}
	//Construct final geometry
	//Add a mesh point for every vertex
	std::vector<UINT> sphereVertInds;
	for (UINT v = 0; v < sphereVerts.size(); v++) {
		float3 vertPos = sphereVerts[v];
		//Calculate UV coordinates
		//Atan returns a range from -pi/2 to pi/2
		float trigMin = -PI / 2;
		float trigMax = PI / 2;

		//Calculate x value (0 - 1 uv coords around the equator of the sphere)
		float xUV = (atan(vertPos.y / vertPos.x) - trigMin) / (trigMax - trigMin);
		xUV = (vertPos.x > 0.0f) ? xUV / 2.0f : 1.0f - (xUV / 2.0f);
		//Calculate y vaule (0 - 1 uv coords mapped from north to south pole)
		float yUV = (atan(vertPos.z / vertPos.x) - trigMin) / (trigMax - trigMin);

		//Put the UV coordinate vector together
		float2 uvCoords = float2(xUV * _uvScale, yUV * _uvScale);
		//Add a vertex
		sphereVertInds.push_back(addVert(vertPos, uvCoords));
	}

	//Construct subdivided sphere
	for (UINT i = 0; i < sphereTris.size(); i++) {
		addTri(sphereVertInds[sphereTris[i][0]], sphereVertInds[sphereTris[i][2]], sphereVertInds[sphereTris[i][1]]);
	}

	//Build the mesh with shared verts and smoothed normals
	buildWithSmoothNormals();
	//Calculate tangents
	CalculateTangents();

	//Create and return the new cube mesh
	Mesh* newMesh = new Mesh(&mVerts[0], vertCount, &mIndices[0], indCount, dxDevice);
	return newMesh;
}
