#include "Game.h"
#include "GameLevel.h"
#include "ResourceManager.h"
#include "Spriterenderer.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "PowerUp.h"

#include "effolkronium/random.hpp"
namespace ek = effolkronium;

Game::Game(unsigned int width, unsigned int height) : state(GameState::GAME_ACTIVE), keys(), width(width), height(height)
{
}

Game::~Game()
{
}

void Game::init()
{
    // load shaders
    ResourceManager::loadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::loadShader("shaders/particle.vert", "shaders/particle.frag", nullptr, "particle");
    ResourceManager::loadShader("shaders/postprocess.vert", "shaders/postprocess.frag", nullptr, "postprocessing");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->width),
        static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
    ResourceManager::getShader("sprite").use().setInteger("image", 0);
    ResourceManager::getShader("sprite").setMatrix4("projection", projection);
    ResourceManager::getShader("particle").use().setInteger("sprite", 0);
    ResourceManager::getShader("particle").setMatrix4("projection", projection);

    // load textures
    ResourceManager::loadTexture("textures/background.jpg", false, "background");
    ResourceManager::loadTexture("textures/awesomeface.png", true, "face");
    ResourceManager::loadTexture("textures/block.png", false, "block");
    ResourceManager::loadTexture("textures/block_solid.png", false, "block_solid");
    ResourceManager::loadTexture("textures/paddle.png", true, "paddle");
    ResourceManager::loadTexture("textures/particle.png", true, "particle");
    ResourceManager::loadTexture("textures/powerup_speed.png", true, "powerup_speed");  
    ResourceManager::loadTexture("textures/powerup_sticky.png", true, "powerup_sticky");
    ResourceManager::loadTexture("textures/powerup_increase.png", true, "powerup_increase");
    ResourceManager::loadTexture("textures/powerup_confuse.png", true, "powerup_confuse");
    ResourceManager::loadTexture("textures/powerup_chaos.png", true, "powerup_chaos");
    ResourceManager::loadTexture("textures/powerup_passthrough.png", true, "powerup_passthrough");
    
    // load levels
    GameLevel one;
    one.load("levels/one.lvl", this->width, this->height / 2);
    GameLevel two;
    two.load("levels/two.lvl", this->width, this->height / 2);
    GameLevel three;
    three.load("levels/three.lvl", this->width, this->height / 2);
    GameLevel four;
    four.load("levels/four.lvl", this->width, this->height / 2);
    this->levels.push_back(one);
    this->levels.push_back(two);
    this->levels.push_back(three);
    this->levels.push_back(four);
    this->level = 0;

    renderer = std::make_unique<SpriteRenderer>(ResourceManager::getShader("sprite"));

    // particle gen
    particles = std::make_unique<ParticleGenerator>(
        ResourceManager::getShader("particle"),
        ResourceManager::getTexture("particle"),
        INITIAL_PARTICLES
    );

    // post procesing
    effects = std::make_unique<PostProcessor>(ResourceManager::getShader("postprocessing"), this->width, this->height);

    // player paddle
    startPlayerPos = glm::vec2(
        this->width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->height - PLAYER_SIZE.y
    );
    player = std::make_unique<GameObject>(startPlayerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));

    // ball
    startBallPos = startPlayerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    ball = std::make_unique<BallObject>(startBallPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face"));
    
}

void Game::processInput(float dt)
{
    if (this->state == GameState::GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->keys[GLFW_KEY_A])
        {
            if (player->position.x >= 0.0f) 
            { 
                player->position.x -= velocity; 
                if (ball->stuck) { ball->position.x -= velocity; }
            }
        }
        if (this->keys[GLFW_KEY_D])
        {
            if (player->position.x <= this->width - player->size.x) 
            { 
                player->position.x += velocity; 
                if (ball->stuck) { ball->position.x += velocity; }
            }
        }
        if (this->keys[GLFW_KEY_SPACE]) { ball->stuck = false; }
    }
}

void Game::update(float dt)
{
    ball->move(dt, this->width);
    this->doCollisionsExist();
    particles->update(dt, *ball, NEW_PARTICLES_PER_FRAME, glm::vec2(ball->radius / 2.0f));
    this->updatePowerUps(dt);
    if (shaketime > 0.f)
    {
        shaketime -= dt;
        if (shaketime <= 0.f) { effects->shake = false; }
    }

    if (ball->position.y >= this->height)
    {
        this->resetPlayer();
        this->resetLevel();
    }
}

void Game::render()
{
    if (this->state == GameState::GAME_ACTIVE)
    {        
        effects->beginRender();

        renderer->drawSprite(ResourceManager::getTexture("background"),
            glm::vec2(0.0f, 0.0f), glm::vec2(this->width, this->height), 0.0f
        );
        this->levels[this->level].draw(*renderer);
        player->draw(*renderer);        
        for (PowerUp& powerUp : this->powerUps)
        {
            if (!powerUp.destroyed) { powerUp.draw(*renderer); }                
        }
        particles->draw();
        ball->draw(*renderer);

        effects->endRender();
        effects->render(glfwGetTime());
    }
}

void Game::resetPlayer()
{
    player->size = PLAYER_SIZE;
    player->position = startPlayerPos;

    ball->stuck = true;
    ball->position = startBallPos;
    ball->velocity = INITIAL_BALL_VELOCITY;
    
    effects->chaos = false;
    effects->confuse = false;
    ball->passthrough = false;
    ball->sticky = false;
    player->color = glm::vec3(1.0f);
    ball->color = glm::vec3(1.0f);
}

void Game::resetLevel()
{
    if (this->level == 0) { this->levels[0].load("levels/one.lvl", this->width, this->height / 2); }
    else if (this->level == 1) { this->levels[1].load("levels/two.lvl", this->width, this->height / 2); }
    else if (this->level == 2) { this->levels[2].load("levels/three.lvl", this->width, this->height / 2); }
    else if (this->level == 3) { this->levels[3].load("levels/four.lvl", this->width, this->height / 2); }
        
}

void Game::doCollisionsExist()
{
    for (GameObject& box : this->levels[this->level].Bricks)
    {
        if (!box.destroyed)
        {
            Collision collision = checkCollision(*ball, box);
            if (collision.isCollision) // if collision is true
            {
                // destroy block if not solid
                if (!box.isSolid) 
                { 
                    box.destroyed = true; 
                    this->spawnPowerUps(box);
                }
                else
                {
                    shaketime = 0.08f;
                    effects->shake = true;
                }

                // collision resolution
                Direction dir = collision.direction;
                glm::vec2 diff_vector = collision.diffVec;
                if (!(ball->passthrough && !box.isSolid)) 
                {
                    if (dir == Direction::LEFT || dir == Direction::RIGHT) // horizontal collision
                    {
                        ball->velocity.x = -ball->velocity.x; // reverse horizontal velocity
                        // relocate
                        float penetration = ball->radius - std::abs(diff_vector.x);
                        if (dir == Direction::LEFT) { ball->position.x += penetration; } // move ball to right
                        else { ball->position.x -= penetration; } // move ball to left;

                    }
                    else // vertical collision
                    {
                        ball->velocity.y = -ball->velocity.y; // reverse vertical velocity
                        // relocate
                        float penetration = ball->radius - std::abs(diff_vector.y);
                        if (dir == Direction::UP) { ball->position.y -= penetration; } // move ball back up
                        else { ball->position.y += penetration; } // move ball back down

                    }
                }
            }
        }

        for (PowerUp& powerUp : this->powerUps)
        {
            if (!powerUp.destroyed)
            {
                if (powerUp.position.y >= this->height) { powerUp.destroyed = true; }
                    
                if (checkCollision(*player, powerUp))
                {	// collided with player, now activate powerup
                    activatePowerUp(powerUp);
                    powerUp.destroyed = true;
                    powerUp.activated = true;
                }
            }
        }
    }

    Collision result = checkCollision(*ball, *player);
    if (!ball->stuck && result.isCollision)
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = player->position.x + player->size.x / 2.0f;
        float distance = (ball->position.x + ball->radius) - centerBoard;
        float percentage = distance / (player->size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = ball->velocity;
        ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        ball->velocity.y = -1.0f * abs(ball->velocity.y);
        ball->velocity = glm::normalize(ball->velocity) * glm::length(oldVelocity);
        ball->stuck = ball->sticky;
    }
}

bool Game::checkCollision(GameObject& one, GameObject& two) const // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.position.x + one.size.x >= two.position.x &&
        two.position.x + two.size.x >= one.position.x;
    // collision y-axis?
    bool collisionY = one.position.y + one.size.y >= two.position.y &&
        two.position.y + two.size.y >= one.position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

//bool Game::checkCollision(BallObject& one, GameObject& two) const
//{
//    // get center point circle first 
//    glm::vec2 center(one.position + one.radius);
//    // calculate AABB info (center, half-extents)
//    glm::vec2 aabb_half_extents(two.size.x / 2.0f, two.size.y / 2.0f);
//    glm::vec2 aabb_center(
//        two.position.x + aabb_half_extents.x,
//        two.position.y + aabb_half_extents.y
//    );
//    // get difference vector between both centers
//    glm::vec2 difference = center - aabb_center;
//    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
//    // add clamped value to AABB_center and we get the value of box closest to circle
//    glm::vec2 closest = aabb_center + clamped;
//    // retrieve vector between center circle and closest point AABB and check if length <= radius
//    difference = closest - center;
//    return glm::length(difference) < one.radius;
//}

Collision Game::checkCollision(BallObject& one, GameObject& two) 
{
    // get center point circle first 
    glm::vec2 center(one.position + one.radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.size.x / 2.0f, two.size.y / 2.0f);
    glm::vec2 aabb_center(
        two.position.x + aabb_half_extents.x,
        two.position.y + aabb_half_extents.y
    );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;

    if (glm::length(difference) <= one.radius)
    {        
        return { true, vectorDirection(difference), difference };
    }
    else
    {        
        return { false, Direction::UP, glm::vec2(0.0f, 0.0f) };
    }
}

Direction Game::vectorDirection(glm::vec2 target)
{
    std::array<glm::vec2, 4> compass = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };

    float max = 0.0f;
    unsigned int best_match = -1;

    for (unsigned int i = 0; i < compass.size(); i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Game::spawnPowerUps(GameObject& block)
{
    if (ek::random_static::get<bool>(.013)) // 1 in 75 chance
    {
        this->powerUps.push_back(
            PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position, ResourceManager::getTexture("powerup_speed")
            ));
    }
    if (ek::random_static::get<bool>(.013))
    {
        this->powerUps.push_back(
            PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.position, ResourceManager::getTexture("powerup_sticky")
            ));
    }
    if (ek::random_static::get<bool>(.013))
    {
        this->powerUps.push_back(
            PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.position, ResourceManager::getTexture("powerup_passthrough")
            ));
    }
    if (ek::random_static::get<bool>(.013))
    {
        this->powerUps.push_back(
            PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.position, ResourceManager::getTexture("powerup_increase")
            ));
    }
    if (ek::random_static::get<bool>(.066)) // negative powerups should spawn more often
    {
        this->powerUps.push_back(
            PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.position, ResourceManager::getTexture("powerup_confuse")
            ));
    }
    if (ek::random_static::get<bool>(.066))
    {
        this->powerUps.push_back(
            PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.position, ResourceManager::getTexture("powerup_chaos")
            ));
    }
}

void Game::updatePowerUps(float dt)
{
    for (PowerUp& powerUp : this->powerUps)
    {
        powerUp.position += powerUp.velocity * dt;
        if (powerUp.activated)
        {
            powerUp.duration -= dt;

            if (powerUp.duration <= 0.0f)
            {
                // remove powerup from list (will later be removed)
                powerUp.activated = false;
                // deactivate effects
                if (powerUp.type == "sticky")
                {
                    if (!isOtherPowerUpActive(this->powerUps, "sticky"))
                    {	// only reset if no other PowerUp of type sticky is active
                        ball->sticky = false;
                        player->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "pass-through")
                {
                    if (!isOtherPowerUpActive(this->powerUps, "pass-through"))
                    {	// only reset if no other PowerUp of type pass-through is active
                        ball->passthrough = false;
                        ball->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "confuse")
                {
                    if (!isOtherPowerUpActive(this->powerUps, "confuse"))
                    {	// only reset if no other PowerUp of type confuse is active
                        effects->confuse = false;
                        ball->velocity = INITIAL_BALL_VELOCITY;
                    }
                }
                else if (powerUp.type == "chaos")
                {
                    if (!isOtherPowerUpActive(this->powerUps, "chaos"))
                    {	// only reset if no other PowerUp of type chaos is active
                        effects->chaos = false;
                    }
                    ball->velocity = INITIAL_BALL_VELOCITY;
                }
            }
        }
    }
    this->powerUps.erase(std::remove_if(this->powerUps.begin(), this->powerUps.end(),
        [](const PowerUp& powerUp) { return powerUp.destroyed && !powerUp.activated; }
    ), this->powerUps.end());
}

bool Game::isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const PowerUp& powerUp : powerUps)
    {
        if (powerUp.activated)
        {
            if (powerUp.type == type) { return true; }                
        }
    }
    return false;
}

void Game::activatePowerUp(PowerUp& powerUp)
{
    if (powerUp.type == "speed")
    {
        ball->velocity *= 1.2;
    }
    else if (powerUp.type == "sticky")
    {
        ball->sticky = true;
        player->color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.type == "pass-through")
    {
        ball->passthrough = true;
        ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.type == "pad-size-increase")
    {
        player->size.x += 50;
    }
    else if (powerUp.type == "confuse")
    {
        if (!effects->chaos) { effects->confuse = true; } // only activate if chaos wasn't already active 
        ball->velocity *= 0.5;
    }
    else if (powerUp.type == "chaos")
    {
        if (!effects->confuse) { effects->chaos = true; }
        ball->velocity *= 0.5;
    }
}
