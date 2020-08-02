#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"


/// GameLevel holds all Tiles as part of a Breakout level and 
/// hosts functionality to Load/render levels from the harddisk.
class GameLevel
{
public:
    GameLevel() { }

   
    void load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
    void draw(SpriteRenderer& renderer);
    bool isCompleted();

    std::vector<GameObject> Bricks;
    unsigned int brickCount = 0; // keep track of destroyable bricks to do a quick check if the level is over without iterating over a container

private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

