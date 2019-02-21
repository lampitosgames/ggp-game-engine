#include "stdafx.h"

#include "Texture.h"
#include "WICTextureLoader.h"
#include <iostream>

using namespace DirectX;

Texture::Texture(ResName _uniqueID, ID3D11Device* _device, ID3D11DeviceContext* _context) {
	HRESULT texSuccessfulLoad = CreateWICTextureFromFile(_device, _context, sconvert.from_bytes(_uniqueID).c_str(), 0, &shaderResourceView);
	loadError = false;
	if (texSuccessfulLoad != S_OK) {
		std::cout << "Texture load error! " << _uniqueID << std::endl;
		loadError = true;
	}
}

Texture::~Texture() {
	if (!loadError) {
		shaderResourceView->Release();
	}
}

ID3D11ShaderResourceView* Texture::GetShaderResourceView() {
	return shaderResourceView;
}
