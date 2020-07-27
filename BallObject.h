#pragma once

#include "GameObject.h"

class BallObject : public GameObject
{
public:
  
    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 move(float dt, unsigned int window_width);
    void reset(glm::vec2 position, glm::vec2 velocity);

    // ball state	
    float radius;
    bool stuck;
    bool sticky;
    bool passthrough;
};

