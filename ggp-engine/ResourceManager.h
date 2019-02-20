#ifndef GGP_RESOURCE_MANAGER
#define GGP_RESOURCE_MANAGER

#include <map>
#include <vector>
#include <string>
#include <d3d11.h>
#include <SimpleMath.h>
#include "json.hpp"

#include "MeshGen.h"
//Forward declaration
class Material;
class PBRMaterial;
class Mesh;
class Texture;
struct Vertex;
class SimpleVertexShader;
class SimplePixelShader;

class ResourceManager {
	static ResourceManager* instance;
	static ID3D11Device* dxDevice;
	static ID3D11DeviceContext* dxContext;
	//Helper object for generating meshes
	static MeshGen meshGen;

	//Object storing filepaths with a JSON filestructure
	nlohmann::json res;
	//Map of unique material resources
	std::map<std::string, Material*> materialUIDMap;
	//Map of unique mesh resources
	std::map<std::string, Mesh*> meshUIDMap;
	//Map of unique texture resources
	std::map<FileName, Texture*> textureUIDMap;
	//Map of unique vertex shaders
	std::map<FileName, SimpleVertexShader*> vertexShaderUIDMap;
	//Map of unique pixel shaders
	std::map<FileName, SimplePixelShader*> pixelShaderUIDMap;
public:
	static ResourceManager* GetInstance();
	static void ReleaseInstance();

	// We don't want anything making copies of this class so delete these operators
	ResourceManager(ResourceManager const&) = delete;
	void operator=(ResourceManager const&) = delete;

	//Sets the device and context needed to actually interact with the window
	static void SetDevicePointer(ID3D11Device* _dxDevice);
	static void SetContextPointer(ID3D11DeviceContext* _dxContext);
	static ID3D11Device* GetDevicePointer();
	static ID3D11DeviceContext* GetContextPointer();

	//Get the resource structure
	nlohmann::json GetRes();

	/*
		MATERIAL RESOURCE MANAGEMENT
	*/
	//Gets the material with a given UID.  If it doesn't exist, returns a new default material with that id
	Material* GetMaterial(std::string _uniqueID);
	//Creates a material with this unique ID (If the id already exists, that material gets returned).
	//Has space for extra materials.
	Material* AddMaterial(std::string _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), float _specular = 0.0f);
	//Same as above, but it fetches the shaders from their filepaths
	Material* AddMaterial(std::string _uniqueID, FileName _vertexFileFileName, FileName _pixelFileFileName, DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), float _specular = 0.0f);
	//Same as above, but it uses a texture instead of a color
	Material* AddMaterial(std::string _uniqueID, FileName _vertexFileFileName, FileName _pixelFileFileName, FileName _textureFileFileName);
	//Same as above, but has all 3 texture channels (diffuse, normal, specular)
	Material* AddMaterial(std::string _uniqueID, FileName _vertexFileFileName, FileName _pixelFileFileName, FileName _diffuseFileFileName, FileName _normalFileFileName, FileName _specularFileFileName);
	//Create and return a new color material with this UID (If the id already exists, the existing mat gets returned)
	Material* AddMaterial(std::string _uniqueID, DirectX::SimpleMath::Color _color, float _specular = 0.0f);
	//Create and return a new basic texture material without shaders
	Material* AddMaterial(std::string _uniqueID, FileName _textureFileFileName);

	/// <summary>
	/// Load in a DDS texture
	/// </summary>
	/// <param name="_textureFileFileName"></param>
	/// <returns></returns>
	ID3D11ShaderResourceView* LoadSRV_DDS(FileName _textureFileFileName);
	/*
		PBR MATERIAL MANAGEMENT
	*/
	PBRMaterial* GetPBRMaterial(std::string _uniqueID, FileName _vertexShaderFileFileName, FileName _pixelShaderFileFileName, DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), float _roughness = 0.0f, float _metalness = 0.0f);
	PBRMaterial* GetPBRMaterial(std::string _uniqueID, FileName _vertexShaderFileFileName, FileName _pixelShaderFileFileName, FileName _albedoFileFileName, FileName _normalFileFileName, FileName _roughnessFileFileName, FileName _metalnessFileFileName);

	/*
		SHADER MANAGEMENT
	*/
	//Load vertex shader
	SimpleVertexShader* GetVertexShader(FileName _filepath);
	SimplePixelShader* GetPixelShader(FileName _filepath);

	/*
		MESH RESOURCE MANAGEMENT
	*/
	Mesh* CreateMeshFromData(Vertex* _vertexArray, UINT _vertexCount, UINT* _indexArray, UINT _indexCount, std::string _uniqueID = "NA");
	Mesh* GetMesh(std::string _uniqueID);
	Mesh* GetTerrain(std::string _uniqueID, int _resolution, float _heightScale = 50.0f, float _uvScale = 30.0f);
	//void DeleteMesh(std::string _uniqueID);
	Mesh* GenerateCube(float _sideLength, float _uvScale = 1.0f);
	Mesh* GenerateSphere(float _radius, int _subdivs = 4, float _uvScale = 1.0f);

	/*
		TEXTURE RESOURCE MANAGEMENT
	*/
	Texture* GetTexture(FileName _filepath);
private:
	ResourceManager();
	~ResourceManager();
	void Release();

	/*
		Mesh loading/building variable storage
	*/
	//Temporary storage for loading vertex data
	std::vector<DirectX::SimpleMath::Vector3> mPositions;
	std::vector<DirectX::SimpleMath::Vector3> mNormals;
	std::vector<DirectX::SimpleMath::Vector2> mUVs;
	//Temporary storage for final constructed vertices/indices
	std::vector<Vertex> mVerts;
	std::vector<UINT> mIndices;

	//Private function to load a mesh from a file
	Mesh* LoadMesh(std::string _filepath);
    Mesh* LoadMeshOBJ(std::string _filepath );
    Mesh* LoadMeshFBX(std::string _filepath );

	void CalculateTangents(int numVerts, int numIndices);
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
