//For use in all pixel shaders. Provides bindings for textures

//TEXTURE REGISTERS
SamplerState basicSampler : register(s0);
Texture2D diffuseTexture  : register(t0);
Texture2D normalMap       : register(t1);
Texture2D specularMap     : register(t2);