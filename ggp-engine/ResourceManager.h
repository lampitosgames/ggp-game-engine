#ifndef GGP_RESOURCE_MANAGER
#define GGP_RESOURCE_MANAGER

#include <map>
#include <vector>
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
		SHADER MANAGEMENT
	*/
	//Load vertex shader
	SimpleVertexShader* GetVertexShader(LPCWSTR _filepath);
	SimplePixelShader* GetPixelShader(LPCWSTR _filepath);

	/*
		MESH RESOURCE MANAGEMENT
		TODO: Loading mesh resources
	*/
	Mesh* CreateMeshFromData(Vertex* _vertexArray, UINT _vertexCount, UINT* _indexArray, UINT _indexCount, std::string _uniqueID = "NA");
	Mesh* GetMesh(std::string _uniqueID);
	//void DeleteMesh(std::string _uniqueID);
private:
	ResourceManager();
	~ResourceManager();
	void Release();

	/*
		Mesh loading/building variable storage
	*/
	//Temporary storage for loading vertex data
	std::vector<DirectX::XMFLOAT3> mPositions;
	std::vector<DirectX::XMFLOAT3> mNormals;
	std::vector<DirectX::XMFLOAT2> mUVs;
	//Temporary storage for final constructed vertices/indices
	std::vector<Vertex> mVerts;
	std::vector<UINT> mIndices;

	//Private function to load a mesh from a file
	Mesh* LoadMesh(std::string _filepath);

	//Mesh* GenerateCube(float _size);
	//Mesh* GenerateSphere(float _radius, int _subdivisions);
};

//Enum of mesh primitive types
enum MeshPrimitives {
	SPHERE = 0,
	CUBE = 1,
	CUBOID = 2,
	CONE = 3,
	CYLINDER = 4,
	TUBE = 5,
	TORUS = 6
};

#endif //GGP_RESOURCE_MANAGER