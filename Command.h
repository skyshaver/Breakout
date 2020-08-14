#pragma once

#include "GameObject.h"
#include "PlayerObject.h"

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(GameObject& gameObject, float dt) = 0;
};


class MoveLeftCommand : public Command
{
public:
	virtual void execute(GameObject& gameObject, float dt) { dynamic_cast<PlayerObject&>(gameObject).moveLeft(dt); }

};

class MoveRightCommand : public Command
{
public:
	virtual void execute(GameObject& gameObject, float dt) { dynamic_cast<PlayerObject&>(gameObject).moveRight(dt); }
};

//class FireCommand : public Command
//{
//public:
//	virtual void execute() { fireGun(); }
//};
