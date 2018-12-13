stoneEntity = {}
spawnedBall = {}
posY = 0;

-----------------------------------------------
--- Called from C++ when we are initialized ---
-----------------------------------------------
function start()
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
  barrel:SetPos(2,-0.75,0)

  barrel2 = Entity.new(
    "barrel2 Lua Entity",             -- Name
    "assets/meshes/barrell.obj", -- Mesh file
    scene,						-- active scene
    barrelMat					-- material
  );
  barrel2:SetPos(2,-0.75,1)

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
    "Lua table",
    "assets/meshes/trn_TableLong.obj",
    scene,
    tableMat
  );

  table:SetPos(0, -0.75, 0)

  ------- cauldron --------------------------------------------------------------
  cauldronMat = Material.new(
    "cauldronMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/cauldron/trn_Cauldron_AlbedoTransparency.png",
    "assets/textures/PBR/cauldron/trn_Cauldron_Normal.png",
    "assets/textures/PBR/cauldron/trn_Cauldron_MetallicSmoothness.png",
    "assets/textures/PBR/cauldron/trn_Cauldron_MetallicSmoothness.png"
  );

  -- Create an entity
  cauldron = Entity.new(
    "Lua cauldron",                     -- Name
    "assets/meshes/trn_Cauldron.obj",   -- Mesh file
    scene,						    -- active scene
    cauldronMat					    -- material
  );

  cauldron:SetPos(-4, -0.75, 0)

  ------- ceramCup --------------------------------------------------------------
  ceramCupMat = Material.new(
    "ceramCupMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/cup_ceramic/trn_CupCeramic_AlbedoTransparency.png",
    "assets/textures/PBR/cup_ceramic/trn_CupCeramic_Normal.png",
    "assets/textures/PBR/cup_ceramic/trn_CupCeramic_MetallicSmoothness.png",
    "assets/textures/PBR/cup_ceramic/trn_CupCeramic_MetallicSmoothness.png"
  );

  -- Create an entity
  ceramCup = Entity.new(
    "Lua ceramCup",                     -- Name
    "assets/meshes/trn_CupCeramic.obj",   -- Mesh file
    scene,						    -- active scene
    ceramCupMat					    -- material
  );

  ceramCup:SetPos(0, 0.1, 0)

  ------- candleCup --------------------------------------------------------------
  candleCupMat = Material.new(
    "Lua candleCupMat 2",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/candle_cup/trn_CandleCup_AlbedoTransparency.png",
    "assets/textures/PBR/candle_cup/trn_CandleCup_Normal.png",
    "assets/textures/PBR/candle_cup/trn_CandleCup_MetallicSmoothness.png",
    "assets/textures/PBR/candle_cup/trn_CandleCup_MetallicSmoothness.png"
  );

  -- Create an entity
  candleCup = Entity.new(
    "Lua candleCup boi",                     -- Name
    "assets/meshes/trn_CandleCup.obj",   -- Mesh file
    scene,						    -- active scene
    candleCupMat					    -- material
  );

  candleCup:SetPos(-0.5, 0.1, 0)

  --- Wood Floor ----------------------------------------------------------------
  woodFloorMat = Material.new(
    "woodFloorMat",
    "VertexShader.cso",
    "PBRPShader.cso",
    "assets/textures/PBR/WoodSiding02_col.jpg",
    "assets/textures/PBR/WoodSiding02_nrm.jpg",
    "assets/textures/PBR/WoodSiding02_rgh.jpg",
    "assets/textures/PBR/WoodSiding02_met.jpg"
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

  wall4 = Entity.new( "Wood Floor wall4", "assets/meshes/cube.obj", scene, woodFloorMat);
  wall4:SetPos(0, 4.5, -7.5)
  wall4:SetScale(15, 15, 0.5)


  -- Create an entity that will move when the user clicks
  spawnedBall = Entity.new("Spawn Ball Boi", "assets/meshes/sphere.obj", scene, woodFloorMat);

  spawnedBall:SetPos(1000,0,0)  -- spawn the ball out in the world somewhere
  spawnedBall:SetScale(0.3, 0.3, 0.3)

end


-----------------------------------
--- Called from C++ every frame ---
-----------------------------------
---function update( dt )
---
---end

-----------------------------------
--------- Called on click ---------
-----------------------------------
function onClick()

    posY = posY + 1
    spawnedBall:SetPos(0, posY, 0)

end
