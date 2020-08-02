#include "PowerUpManager.h"
#include "ResourceManager.h"
#include "BallObject.h"
#include "PostProcessor.h"

#include "effolkronium/random.hpp"
namespace ek = effolkronium;

PowerUpManager::PowerUpManager()
{
}

void PowerUpManager::update(float dt, BallObject& ball, GameObject& player, PostProcessor& effects)
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
                        ball.sticky = false;
                        player.color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "pass-through")
                {
                    if (!isOtherPowerUpActive(this->powerUps, "pass-through"))
                    {	// only reset if no other PowerUp of type pass-through is active
                        ball.passthrough = false;
                        ball.color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "confuse")
                {
                    if (!isOtherPowerUpActive(this->powerUps, "confuse"))
                    {	// only reset if no other PowerUp of type confuse is active
                        effects.confuse = false;
                        ball.velocity = BALL_INITIAL_VELOCITY;
                    }
                }
                else if (powerUp.type == "chaos")
                {
                    if (!isOtherPowerUpActive(this->powerUps, "chaos"))
                    {	// only reset if no other PowerUp of type chaos is active
                        effects.chaos = false;
                    }
                    ball.velocity = BALL_INITIAL_VELOCITY;
                }
            }
        }
    }
    this->powerUps.erase(std::remove_if(this->powerUps.begin(), this->powerUps.end(),
        [](const PowerUp& powerUp) { return powerUp.destroyed && !powerUp.activated; }
    ), this->powerUps.end());
}

void PowerUpManager::spawnPowerUps(GameObject& block)
{
    if (ek::random_static::get<bool>(POWERUP_GOOD_CHANCE)) // 1 in 75 chance
    {
        this->powerUps.push_back(
            PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position, ResourceManager::getTexture("powerup_speed")
            ));
    }
    if (ek::random_static::get<bool>(POWERUP_GOOD_CHANCE))
    {
        this->powerUps.push_back(
            PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.position, ResourceManager::getTexture("powerup_sticky")
            ));
    }
    if (ek::random_static::get<bool>(POWERUP_GOOD_CHANCE))
    {
        this->powerUps.push_back(
            PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.position, ResourceManager::getTexture("powerup_passthrough")
            ));
    }
    if (ek::random_static::get<bool>(POWERUP_GOOD_CHANCE))
    {
        this->powerUps.push_back(
            PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.position, ResourceManager::getTexture("powerup_increase")
            ));
    }
    if (ek::random_static::get<bool>(POWERUP_BAD_CHANCE)) // negative powerups should spawn more often, 1 in 15
    {
        this->powerUps.push_back(
            PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.position, ResourceManager::getTexture("powerup_confuse")
            ));
    }
    if (ek::random_static::get<bool>(POWERUP_BAD_CHANCE))
    {
        this->powerUps.push_back(
            PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.position, ResourceManager::getTexture("powerup_chaos")
            ));
    }
}

void PowerUpManager::activatePowerUps(PowerUp& powerUp, BallObject& ball, GameObject& player, PostProcessor& effects)
{
    if (powerUp.type == "speed")
    {
        ball.velocity *= 1.2;
    }
    else if (powerUp.type == "sticky")
    {
        ball.sticky = true;
        player.color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.type == "pass-through")
    {
        ball.passthrough = true;
        ball.color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.type == "pad-size-increase")
    {
        player.size.x += 50;
    }
    else if (powerUp.type == "confuse")
    {
        if (!effects.chaos) { effects.confuse = true; } // only activate if chaos wasn't already active 
        ball.velocity *= 0.5;
    }
    else if (powerUp.type == "chaos")
    {
        if (!effects.confuse) { effects.chaos = true; }
        ball.velocity *= 0.5;
    }
}

bool PowerUpManager::isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const auto& powerUp : powerUps)
    {
        if (powerUp.activated)
        {
            if (powerUp.type == type) { return true; }
        }
    }
    return false;
}
