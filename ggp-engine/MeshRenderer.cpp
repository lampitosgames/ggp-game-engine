#include "stdafx.h"

#include "MeshRenderer.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Mesh.h"
#include "Spatial.h"

MeshRenderer::MeshRenderer(UINT _uniqueID, Spatial* _gameObject) {
	type = CompType::MESH_RENDERER;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	mesh = nullptr;
	material = nullptr;
}

MeshRenderer::MeshRenderer(UINT _uniqueID, Spatial* _gameObject, Mesh* _mesh, Material* _material) {
	type = CompType::MESH_RENDERER;
	uniqueID = _uniqueID;
	gameObject = _gameObject;
	mesh = _mesh;
	material = _material;
}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::SetMesh(Mesh* _mesh) {
	mesh = _mesh;
}

void MeshRenderer::SetMaterial(Material* _material) {
	material = _material;
}

void MeshRenderer::Draw(ID3D11DeviceContext* _dxContext) {
	//Should only be called after shaders and transformations have been set.
	//Store mesh data in local variables
	UINT thisStride = mesh->GetStride();
	UINT thisOffset = mesh->GetOffset();
	ID3D11Buffer* thisVertexBuffer = mesh->GetVertexBuffer();
	//Set the active vertex/index buffers to this mesh's
	_dxContext->IASetVertexBuffers(0, 1, &thisVertexBuffer, &thisStride, &thisOffset);
	_dxContext->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	//Draw the mesh. (# indices to use, first index, offset for each index)
	_dxContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	return;
}

DirectX::XMFLOAT4X4 MeshRenderer::GetWorldMatrix() {
	return gameObject->transform.GetWorldMatrix();
}

DirectX::XMFLOAT4X4 MeshRenderer::GetWorldInvTransMatrix() {
	return gameObject->transform.GetWorldInvTransMatrix();
}

Mesh* MeshRenderer::GetMesh() {
	return mesh;
}

Material* MeshRenderer::GetMaterial() {
	return material;
}

SimpleVertexShader* MeshRenderer::GetVertexShader() {
	if (material == nullptr) { return nullptr; }
	return material->GetVertexShader();
}

SimplePixelShader* MeshRenderer::GetPixelShader() {
	if (material == nullptr) { return nullptr; }
	return material->GetPixelShader();
}

DirectX::XMFLOAT4 MeshRenderer::GetColor() {
	if (material == nullptr) { return DirectX::XMFLOAT4(); }
	return material->GetColor();
}
