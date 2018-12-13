stoneEntity = {}

posY = 0;

-----------------------------------------------
--- Called from C++ when we are initialized ---
-----------------------------------------------
function start()

  -- Create a new material
  stoneMat = Material.new(
	"LuaMaterial",
    "VertexShader.cso",                     --FileName vertexShader
    "PBRPShader.cso",							          --FileName pixelShader
    "assets/textures/PBR/bronze_a.png",			--FileName albedoTexture
    "assets/textures/PBR/bronze_n.png",			--FileName normalTexture
    "assets/textures/PBR/bronze_r.png",			--FileName RoughnessTexture
    "assets/textures/PBR/bronze_m.png"		  --FileName MetalTexture
  );

  -- Create a new material
  barrelMat = Material.new(
    "barrelMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/barrel/trn_Barrel_AlbedoTransparency.png",
    "assets/textures/PBR/barrel/trn_Barrel_Normal.png",
    "assets/textures/PBR/barrel/trn_BarrelStand_MetallicSmoothness.png",
    "assets/textures/PBR/barrel/trn_Barrel_MetallicSmoothness.png"
  );

  -- Create an entity
  barrel = Entity.new(
    "barrel Lua Entity",             -- Name
    "assets/meshes/barrell.obj", -- Mesh file
    scene,						-- active scene
    barrelMat					-- material
  );

  -- Create an entity
  stoneEntity = Entity.new(
    "Lua Stone Entity",             -- Name
    "assets/meshes/sphere.obj", -- Mesh file
    scene,						-- active scene
    stoneMat					-- material
  );

  stoneEntity:SetPos(0, 2, 0)
  barrel:SetPos(1,-1,0)

end




-----------------------------------
--- Called from C++ every frame ---
-----------------------------------
---function update( dt )
---
---end
