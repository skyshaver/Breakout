#pragma once

#include "GameObject.h"
#include "PlayerObject.h"

class Command
{
public:
	virtual ~Command() {}
	virtual void execute(PlayerObject& gameObject, float dt) = 0;
};


class MoveLeftCommand : public Command
{
public:
	virtual void execute(PlayerObject& gameObject, float dt) { gameObject.moveLeft(dt); }
};

class MoveRightCommand : public Command
{
public:
	virtual void execute(PlayerObject& gameObject, float dt) { gameObject.moveRight(dt); }
};

//class FireCommand : public Command
//{
//public:
//	virtual void execute() { fireGun(); }
//};
