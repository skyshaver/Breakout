#pragma once

#include "GameObject.h"

#include "glm/glm.hpp"

class PlayerObject : public GameObject
{
	using GameObject::GameObject;

public:
	void moveRight(float dt);
	void moveLeft(float dt);
	void move(float dt, glm::vec2 dir);
};

