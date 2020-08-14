#include "PlayerObject.h"
#include "GlobalConstants.h"

void PlayerObject::moveRight(float dt)
{
    float velocity = PLAYER_VELOCITY * dt;
    if (this->position.x >= 0.0f)
    {
        this->position.x -= velocity;
        //if (ball->stuck) { ball->position.x -= velocity; }
    }
}

void PlayerObject::moveLeft(float dt)
{
    float velocity = PLAYER_VELOCITY * dt;
    if (this->position.x <= SCREEN_WIDTH - this->size.x)
    {
        this->position.x += velocity;
        //if (ball->stuck) { ball->position.x += velocity; }
    }
}
