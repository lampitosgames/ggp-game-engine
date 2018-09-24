#ifndef GGP_RESOURCE_MANAGER
#define GGP_RESOURCE_MANAGER

#include <map>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
class Material;
class Mesh;
struct Vertex;
class SimpleVertexShader;
class SimplePixelShader;

class ResourceManager {
	static ResourceManager* instance;
	static ID3D11Device* dxDevice;
	static ID3D11DeviceContext* dxContext;

	//Map of unique material resources
	std::map<std::string, Material*> materialUIDMap;
	//Map of unique mesh resources
	std::map<std::string, Mesh*> meshUIDMap;
	//Map of unique vertex shaders
	std::map<LPCWSTR, SimpleVertexShader*> vertexShaderUIDMap;
	//Map of unique pixel shaders
	std::map<LPCWSTR, SimplePixelShader*> pixelShaderUIDMap;
public:
	static ResourceManager* GetInstance();
	static void ReleaseInstance();
	//Sets the device and context needed to actually interact with the window
	static void SetDevicePointer(ID3D11Device* _dxDevice);
	static void SetContextPointer(ID3D11DeviceContext* _dxContext);

	/*
		MATERIAL RESOURCE MANAGEMENT
	*/
	//Gets the material with a given UID.  If it doesn't exist, returns a new default material with that id
	Material* GetMaterial(std::string _uniqueID);
	//Creates a material with this unique ID (If the id already exists, that material gets returned).
	//Has space for extra materials.
	Material* AddMaterial(std::string _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//Same as above, but it fetches the shaders from their filepaths
	Material* AddMaterial(std::string _uniqueID, LPCWSTR _vertexFilestring, LPCWSTR _pixelFilestring, DirectX::XMFLOAT4 _color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//Create and return a new color material with this UID (If the id already exists, the existing mat gets returned)
	Material* AddMaterial(std::string _uniqueID, DirectX::XMFLOAT4 _color);

	/*
		MESH RESOURCE MANAGEMENT
		TODO: Loading mesh resources
	*/
	Mesh* CreateMeshFromData(Vertex _vertexArray[], UINT _vertexCount, int _indexArray[], UINT _indexCount, std::string _uniqueID = "NA");
	//Mesh* LoadMesh(std::string _filepath, std::string _uniqueID = "NA");
	//Mesh* GetMesh(std::string _uniqueID);
	//void DeleteMesh(std::string _uniqueID);

	/*
		SHADER MANAGEMENT
	*/
	//Load vertex shader
	SimpleVertexShader* GetVertexShader(LPCWSTR _filepath);
	SimplePixelShader* GetPixelShader(LPCWSTR _filepath);
private:
	ResourceManager();
	~ResourceManager();
	void Release();
};

#endif //GGP_RESOURCE_MANAGER