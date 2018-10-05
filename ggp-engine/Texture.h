#ifndef GGP_TEXTURE_H
#define GGP_TEXTURE_H

#include <WICTextureLoader.h>

class Texture {
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11Resource* textureRes;
	ID3D11SamplerState* textureSamplerState;
public:
	Texture(LPCWSTR _uniqueID, ID3D11Device* _device, ID3D11DeviceContext* _context);

	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11Resource* GetTexResource();
	ID3D11SamplerState* GetSamplerState();
};

#endif //GGP_TEXTURE_H