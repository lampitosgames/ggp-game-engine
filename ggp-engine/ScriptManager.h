#pragma once

#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>

#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "PBRMaterial.h"

namespace Scripting
{

    /// <summary>
    /// Class that controls the loading of Lua script states
    /// </summary>
    /// <author>Ben Hoffman</author>
    class ScriptManager
    {


    public:

        /// <summary>
        /// Create a scripting manager with the given device and context for
        /// creating entities and materials
        /// </summary>
        /// <param name="aDevice">Device to use for material/shader creation</param>
        /// <param name="aContext">Context to use for material/shader creation</param>
        ScriptManager( ID3D11Device* aDevice, ID3D11DeviceContext* aContext );

        ~ScriptManager();

        /// <summary>
        /// Update any entity callbacks
        /// </summary>
        /// <param name="deltaTime"></param>
        void Update( float deltaTime );

        /// <summary>
        /// Call any lua scripts that have an OnClick funtion
        /// </summary>
        void OnClick();

        /// <summary>
        /// Load all scripts in the Assets/Scripts directory
        /// </summary>
        void LoadScripts( Scene* aScene );

    private:


        /// <summary>
        /// Load in this lua script and store it's lua state
        /// </summary>
        /// <param name="aFile"></param>
        void LoadScript( const char* aFile, Scene* aScene );

        /// <summary>
        /// Define the lua states for any game play scripts
        /// </summary>
        /// <param name="aLua">the lua state to edit</param>
        void DefinedLuaTypes( sol::state & aLua, Scene* aScene );

        /** Lua update function callbacks */
        std::vector<sol::function> UpdateTicks;

        /** Lua update function callbacks */
        std::vector<sol::function> OnClickFuncs;

        /** Lua states that should be persistent */
        std::vector<sol::state> LuaStates;

        /** Target device to create shaders/materials on */
        ID3D11Device* Device;

        /** Target context to create shaders/materials on */
        ID3D11DeviceContext* Context;


        /***********************************************************/
        /* Creation data definitions                               */
        /***********************************************************/


        struct MaterialCreationData
        {
            MaterialCreationData(
                std::string aName,
                FileName vertexShader,
                FileName pixelShader,
                FileName albedoTexture,
                FileName normalTexture,
                FileName roughnessTexture,
                FileName metalTexture
            )
            {
                DEBUG_PRINT( "Material Creation" );
                ResourceManager* resourceMan = ResourceManager::GetInstance();

                material = resourceMan->GetPBRMaterial(
                    aName,
                    vertexShader.c_str(),
                    pixelShader.c_str(),
                    albedoTexture.c_str(),
                    normalTexture.c_str(),
                    roughnessTexture.c_str(),
                    metalTexture.c_str()
                );
            }

            ~MaterialCreationData()
            { 
                material = nullptr;
            }

            PBRMaterial* material = nullptr;
        };

        // Create the entity type
        struct EntityCreationData
        {
            EntityCreationData( 
                std::string aName, 
                std::string aMeshName, 
                Scene* activeScene,
                MaterialCreationData* matData )
            {
                MeshName = aMeshName;

                ResourceManager* resMan = ResourceManager::GetInstance();

                Mesh* mesh = resMan->GetMesh( MeshName );

                CreatedEntity = new GameObject( aName );
                activeScene->AddChild( CreatedEntity );

                MeshRenderer* aMeshRend = 
                    CreatedEntity->AddComponent<MeshRenderer>( CreatedEntity );

                aMeshRend->SetMesh( mesh );
                aMeshRend->SetMaterial( matData->material );

                DEBUG_PRINT( "Load Complete!" );

            }

            ~EntityCreationData()
            {
                CreatedEntity = nullptr;
            }

            void SetPos( const float x, const float y, const float z )
            {
                assert( CreatedEntity != nullptr );

                DirectX::XMFLOAT3 newPos( x, y, z );
                CreatedEntity->transform.position = newPos;
            }

            void SetScale( const float x, const float y, const float z )
            {
                assert( CreatedEntity != nullptr );

                DirectX::XMFLOAT3 newScale( x, y, z );
                CreatedEntity->transform.scale = newScale;
            }

        private:

            /** Info to create the entity */
            std::string VertexShaderFile;
            std::string PixelShaderFile;

            /** Pointer to the entity that was created */
            GameObject* CreatedEntity = nullptr;

            /** The file name of this created entity in wide format */
            std::string MeshName;
        };

    };

}   // namespace Scripting