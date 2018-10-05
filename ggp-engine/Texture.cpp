#include "Texture.h"
#include "WICTextureLoader.h"
#include <iostream>

using namespace DirectX;

Texture::Texture(LPCWSTR _uniqueID, ID3D11Device* _device, ID3D11DeviceContext* _context) {
	HRESULT texSuccessfulLoad = CreateWICTextureFromFile(_device, _context, _uniqueID, &textureRes, &shaderResourceView);
	if (texSuccessfulLoad == S_OK) {
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		HRESULT samplerSuccessfulLoad = _device->CreateSamplerState(&samplerDesc, &textureSamplerState);
		if (!samplerSuccessfulLoad) {
			std::cout << "Sampler load error! " << _uniqueID << std::endl;
		}
	} else {
		std::cout << "Texture load error! " << _uniqueID << std::endl;
	}
}

ID3D11ShaderResourceView* Texture::GetShaderResourceView() {
	return shaderResourceView;
}

ID3D11Resource* Texture::GetTexResource() {
	return textureRes;
}

ID3D11SamplerState* Texture::GetSamplerState() {
	return textureSamplerState;
}
