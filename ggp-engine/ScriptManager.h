#pragma once

#define SOL_CHECK_ARGUMENTS 1
#include <sol.hpp>
#include <d3d11.h>

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
        /// Load all scripts in the Assets/Scripts directory
        /// </summary>
        void LoadScripts();

    private:


        /// <summary>
        /// Load in this lua script and store it's lua state
        /// </summary>
        /// <param name="aFile"></param>
        void LoadScript( const char* aFile );

        /// <summary>
        /// Define the lua states for any game play scripts
        /// </summary>
        /// <param name="aLua">the lua state to edit</param>
        void DefinedLuaTypes( sol::state & aLua );

        /** Lua update function callbacks */
        std::vector<sol::function> UpdateTicks;

        /** Lua states that should be persistent */
        std::vector<sol::state> LuaStates;

        /** Target device to create shaders/materials on */
        ID3D11Device* Device;

        /** Target context to create shaders/materials on */
        ID3D11DeviceContext* Context;
    };

}   // namespace Scripting