#ifndef GGP_TEXTURE_H
#define GGP_TEXTURE_H

#include <WICTextureLoader.h>

class Texture {
	ID3D11ShaderResourceView* shaderResourceView;
	bool loadError;
public:
	Texture(LPCWSTR _uniqueID, ID3D11Device* _device, ID3D11DeviceContext* _context);
	~Texture();

	ID3D11ShaderResourceView* GetShaderResourceView();
};

#endif //GGP_TEXTURE_H