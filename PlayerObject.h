#pragma once

#include "GameObject.h"

class PlayerObject : public GameObject
{
	using GameObject::GameObject;

public:
	void moveRight(float dt);
	void moveLeft(float dt);
};

