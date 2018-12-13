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

  ------- Barrel -------------------------------------------------------------
  barrelMat = Material.new(
    "barrelMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/barrel/trn_Barrel_AlbedoTransparency.png",
    "assets/textures/PBR/barrel/trn_Barrel_Normal.png",
    "assets/textures/PBR/barrel/trn_BarrelStand_MetallicSmoothness.png",
    "assets/textures/PBR/barrel/trn_Barrel_MetallicSmoothness.png"
  );

  barrel = Entity.new(
    "barrel Lua Entity",             -- Name
    "assets/meshes/barrell.obj", -- Mesh file
    scene,						-- active scene
    barrelMat					-- material
  );
  barrel:SetPos(1,-1,0)

  ------- Lamp --------------------------------------------------------------
  lampMat = Material.new(
    "lampMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/lamp/trn_Lamp_AlbedoTransparency.png",
    "assets/textures/PBR/lamp/trn_Lamp_Normal.png",
    "assets/textures/PBR/lamp/trn_Lamp_MetallicSmoothness.png",
    "assets/textures/PBR/lamp/trn_Lamp_MetallicSmoothness.png"
  );

  lamp = Entity.new(
    "Lua Lamp",                     -- Name
    "assets/meshes/trn_Lamp.obj",   -- Mesh file
    scene,						    -- active scene
    lampMat					    -- material
  );

  ------- Table --------------------------------------------------------------
  tableMat = Material.new(
    "tableMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/table/trn_TableLong_AlbedoTransparency.png",
    "assets/textures/PBR/table/trn_TableLong_Normal.png",
    "assets/textures/PBR/table/trn_TableLong_MetallicSmoothness.png",
    "assets/textures/PBR/table/trn_TableLong_MetallicSmoothness.png"
  );

  -- Create an entity
  table = Entity.new(
    "Lua table",                     -- Name
    "assets/meshes/trn_TableLong.obj",   -- Mesh file
    scene,						    -- active scene
    lampMat					    -- material
  );

  table:SetPos(-1, 0, 0)

  --- Wood Floor ----------------------------------------------------------------
  woodFloorMat = Material.new(
    "woodFloorMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/wood_albedo.png",
    "assets/textures/PBR/wood_normals.png",
    "assets/textures/PBR/wood_roughness.png",
    "assets/textures/PBR/wood_metal.png"
  );

  -- Create an entity
  floor = Entity.new(
    "Wood Floor table",
    "assets/meshes/cube.obj",
    scene,
    woodFloorMat
  );

  floor:SetPos(0, -1, 0)
  floor:SetScale(15, 0.5, 15)

  -- Create an entity
  roof = Entity.new(
    "Wood roof",
    "assets/meshes/cube.obj",
    scene,
    woodFloorMat
  );

  roof:SetPos(0, 7.5, 0)
  roof:SetScale(15, 0.5, 15)
  --------- Walls -----------------
  -- Create an entity
  wall1 = Entity.new( "Wood Floor wall1", "assets/meshes/cube.obj", scene, woodFloorMat);
  wall1:SetPos(-7.5, 4.5, 0)
  wall1:SetScale(0.5, 15, 15)

  wall2 = Entity.new( "Wood Floor wall2", "assets/meshes/cube.obj", scene, woodFloorMat);
  wall2:SetPos(7.5, 4.5, 0)
  wall2:SetScale(0.5, 15, 15)

  wall3 = Entity.new( "Wood Floor wall3", "assets/meshes/cube.obj", scene, woodFloorMat);
  wall3:SetPos(0, 4.5, 7.5)
  wall3:SetScale(15, 15, 0.5)

  wall3 = Entity.new( "Wood Floor wall3", "assets/meshes/cube.obj", scene, woodFloorMat);
  wall3:SetPos(0, 4.5, -7.5)
  wall3:SetScale(15, 15, 0.5)

end


-----------------------------------
--- Called from C++ every frame ---
-----------------------------------
---function update( dt )
---
---end
