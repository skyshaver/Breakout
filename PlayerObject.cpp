#include "PlayerObject.h"
#include "GlobalConstants.h"


// dir vector
// 1, 0 right
// -1 , 0 left
// 0, 1 up
// 0, -1 down
void PlayerObject::move(float dt, glm::vec2 dir)
{
    float velocity = PLAYER_VELOCITY * dt;
    if (dir.x > 0)
    {
        if (this->position.x <= SCREEN_WIDTH - this->size.x)
        {
            this->position.x += velocity;
            //if (ball->stuck) { ball->position.x += velocity; }
        }
    }
    else
    {

        if (this->position.x >= 0.0f)
        {
            this->position.x -= velocity;
            //if (ball->stuck) { ball->position.x -= velocity; }
        }
    }
}

void PlayerObject::moveRight(float dt)
{
    float velocity = PLAYER_VELOCITY * dt;
    if (this->position.x <= SCREEN_WIDTH - this->size.x)
    {
        this->position.x += velocity;
        //if (ball->stuck) { ball->position.x += velocity; }
    }
}

void PlayerObject::moveLeft(float dt)
{
    float velocity = PLAYER_VELOCITY * dt;
    if (this->position.x >= 0.0f)
    {
        this->position.x -= velocity;
        //if (ball->stuck) { ball->position.x -= velocity; }
    }
}