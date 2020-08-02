#pragma once

#include <vector>

#include "PowerUp.h"
#include "GlobalConstants.h"

class BallObject;
class PostProcessor;

class PowerUpManager
{
	PowerUpManager();

	void update(float dt, BallObject& ball, GameObject& player, PostProcessor& effects);
	void spawnPowerUps(GameObject& block);
	void activatePowerUps(PowerUp& powerUp, BallObject& ball, GameObject& player, PostProcessor& effects);

	bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);

	std::vector<PowerUp> powerUps;
};

