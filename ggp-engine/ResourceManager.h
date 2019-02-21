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

	//Map of unique material resources
	std::map<ResName, Material*> materialUIDMap;
	//Map of unique mesh resources
	std::map<ResName, Mesh*> meshUIDMap;
	//Map of unique texture resources
	std::map<ResName, Texture*> textureUIDMap;
	//Map of unique vertex shaders
	std::map<ResName, SimpleVertexShader*> vertexShaderUIDMap;
	//Map of unique pixel shaders
	std::map<ResName, SimplePixelShader*> pixelShaderUIDMap;
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

	/*
		MATERIAL RESOURCE MANAGEMENT
	*/
	//Gets the material with a given UID.  If it doesn't exist, returns a new default material with that id
	Material* GetMaterial(ResName _uniqueID);
	//Creates a material with this unique ID (If the id already exists, that material gets returned).
	//Has space for extra materials.
	Material* AddMaterial(ResName _uniqueID, SimpleVertexShader* _vertexShader, SimplePixelShader* _pixelShader, DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), float _specular = 0.0f);
	//Same as above, but it fetches the shaders from their filepaths
	Material* AddMaterial(ResName _uniqueID, ResName _vertexFilestring, ResName _pixelFilestring, DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), float _specular = 0.0f);
	//Same as above, but it uses a texture instead of a color
	Material* AddMaterial(ResName _uniqueID, ResName _vertexFilestring, ResName _pixelFilestring, ResName _textureFilestring);
	//Same as above, but has all 3 texture channels (diffuse, normal, specular)
	Material* AddMaterial(ResName _uniqueID, ResName _vertexFilestring, ResName _pixelFilestring, ResName _diffuseFilestring, ResName _normalFilestring, ResName _specularFilestring);
	//Create and return a new color material with this UID (If the id already exists, the existing mat gets returned)
	Material* AddMaterial(ResName _uniqueID, DirectX::SimpleMath::Color _color, float _specular = 0.0f);
	//Create and return a new basic texture material without shaders
	Material* AddMaterial(ResName _uniqueID, ResName _textureFilestring);

	/// <summary>
	/// Load in a DDS texture
	/// </summary>
	/// <param name="_textureFileString"></param>
	/// <returns></returns>
	ID3D11ShaderResourceView* LoadSRV_DDS(ResName _textureFileString);
	/*
		PBR MATERIAL MANAGEMENT
	*/
	PBRMaterial* GetPBRMaterial(ResName _uniqueID, ResName _vertexShaderFilestring, ResName _pixelShaderFilestring, DirectX::SimpleMath::Color _color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f), float _roughness = 0.0f, float _metalness = 0.0f);
	PBRMaterial* GetPBRMaterial(ResName _uniqueID, ResName _vertexShaderFilestring, ResName _pixelShaderFilestring, ResName _albedoFilestring, ResName _normalFilestring, ResName _roughnessFilestring, ResName _metalnessFilestring);

	/*
		SHADER MANAGEMENT
	*/
	//Load vertex shader
	SimpleVertexShader* GetVertexShader(ResName _filepath);
	SimplePixelShader* GetPixelShader(ResName _filepath);

	/*
		MESH RESOURCE MANAGEMENT
	*/
	Mesh* CreateMeshFromData(Vertex* _vertexArray, UINT _vertexCount, UINT* _indexArray, UINT _indexCount, ResName _uniqueID = ResName("NA"));
	Mesh* GetMesh(ResName _uniqueID);
	Mesh* GetTerrain(ResName _uniqueID, int _resolution, float _heightScale = 50.0f, float _uvScale = 30.0f);
	//void DeleteMesh(std::string _uniqueID);
	Mesh* GenerateCube(float _sideLength, float _uvScale = 1.0f);
	Mesh* GenerateSphere(float _radius, int _subdivs = 4, float _uvScale = 1.0f);

	/*
		TEXTURE RESOURCE MANAGEMENT
	*/
	Texture* GetTexture(ResName _filepath);
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
	Mesh* LoadMesh(ResName _filepath);
    Mesh* LoadMeshOBJ(ResName _filepath );
    Mesh* LoadMeshFBX(ResName _filepath );

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
