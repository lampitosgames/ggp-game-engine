#include "stdafx.h"
#include "ScriptManager.h"

using namespace Scripting;

ScriptManager::ScriptManager( ID3D11Device* aDevice, ID3D11DeviceContext* aContext )
    :Device( aDevice ), Context( aContext )
{
}


ScriptManager::~ScriptManager()
{
    // Clear lua states
    UpdateTicks.clear();
    OnClickFuncs.clear();
    LuaStates.clear();

    // Remove any dangling pointers
    Device = nullptr;
    Context = nullptr;
}

void ScriptManager::Update( float deltaTime )
{
    for ( auto it : UpdateTicks )
    {
        it( deltaTime );
    }
}

void ScriptManager::OnClick()
{
    for ( auto it : OnClickFuncs )
    {
        DEBUG_PRINT( "OnClick!" );
        it();
    }
}

void ScriptManager::LoadScripts( Scene* aScene )
{
    // #TODO 
    // Load all scripts currently here
    LoadScript( "assets/Scripts/test.lua", aScene );
}

void ScriptManager::LoadScript( const char * aFile, Scene* aScene )
{
    sol::state lua;
    lua.open_libraries( sol::lib::base );

    // Set lua types
    DefinedLuaTypes( lua, aScene );

    // Load in this script...
    lua.script_file( aFile );

    // Call start on the script if it has it
    sol::optional <sol::function> unsafe_startFunc = lua [ "start" ];
    if ( unsafe_startFunc != sol::nullopt )
    {
        sol::function& start_function = unsafe_startFunc.value();
        start_function();
    }

    // Store the update function for later if there is one
    sol::optional <sol::function> unsafe_updateFunc = lua [ "update" ];
    if ( unsafe_updateFunc != sol::nullopt )
    {
        sol::function& update_func = unsafe_updateFunc.value();

        UpdateTicks.emplace_back( update_func );
    }

    // Store the update function for later if there is one
    sol::optional <sol::function> unsafe_onclick_func = lua [ "onClick" ];
    if ( unsafe_onclick_func != sol::nullopt )
    {
        sol::function& onclick_func = unsafe_onclick_func.value();

        OnClickFuncs.emplace_back( onclick_func );
    }

    LuaStates.push_back( std::move( lua ) );
}


void ScriptManager::DefinedLuaTypes( sol::state & aLua, Scene* aScene )
{
    aLua [ "device" ] = Device;
    aLua [ "context" ] = Context;
    aLua [ "scene" ] = aScene;

    // Define the entity types
    aLua.new_usertype<EntityCreationData>( "Entity",
        sol::constructors<
        EntityCreationData( 
            std::string aName, 
            std::string aMeshName,
            Scene* activeScene, MaterialCreationData* matData
        )>(),

        "SetPos", &EntityCreationData::SetPos,
        "SetScale", &EntityCreationData::SetScale

        );

    aLua.new_usertype<MaterialCreationData>( "Material",
        sol::constructors<
        MaterialCreationData(
            std::string aName,
            FileName vertexShader,
            FileName pixelShader,
            FileName albedoTexture,
            FileName normalTexture,
            FileName roughnessTexture,
            FileName metalTexture
        )>()

        );

    aLua.new_usertype<DirectX::XMFLOAT3>( "VEC3",
        "x", &DirectX::XMFLOAT3::x,
        "y", &DirectX::XMFLOAT3::y,
        "z", &DirectX::XMFLOAT3::z
        );
}
