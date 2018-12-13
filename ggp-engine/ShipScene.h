#pragma once

#include "Scene.h"

class ShipScene : public Scene
{
public:
    ShipScene( std::string _uniqueID ) : Scene( _uniqueID ) {}

    void Init();
    void Start();
    void Update( float _deltaTime );

private:
    void CreateLights();
};

