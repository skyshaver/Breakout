#include <iostream>
#include <vector>
#include <memory>

struct Point
{
    int x = 0;
    int y = 0;
};


class GameObject
{
public:

    GameObject(){};
    GameObject(int height, int width) : height(height), width(width) {}

    int height;
    int width;
    Point position = {2,3};

    
};

class PlayerObject : public GameObject
{
    using GameObject::GameObject;
public:
    void moveLeft(){ std::cout << "moveLeft" << '\n'; }
    void moveRight(){ std::cout << "moveRight" << '\n'; }
};



void printStuff(GameObject& gameObject)
{

    std::cout << gameObject.height << " " << gameObject.width << " " << gameObject.position.x << " " << gameObject.position.y << '\n';
}

class Command
{
public:
  virtual ~Command() {}
  virtual void execute(PlayerObject& gameObject) = 0;
};

class MoveLeftCommand : public Command
{
public:
	virtual void execute(PlayerObject& gameObject) { gameObject.moveLeft(); }
};

class MoveRightCommand : public Command
{
public:
	virtual void execute(PlayerObject& gameObject) { gameObject.moveRight(); }
};

class InputHandler
{
public:
    InputHandler(){ bindKeys(); }

    Command* handleInput(char key)
    {
        if(key == 'A') { return keyA.get(); }
        else if (key == 'B') { return keyD.get(); }
    }
private:
    void bindKeys()
    {
        keyA = std::make_unique<MoveLeftCommand>();
        keyD = std::make_unique<MoveRightCommand>();
    }
private:

    std::unique_ptr<Command> keyA;
    std::unique_ptr<Command> keyD;
    
};

int main()
{
    PlayerObject player(100, 100);
    printStuff(player);

    std::unique_ptr<PlayerObject> player2 = std::make_unique<PlayerObject>(400,600);

    InputHandler inputHandler;
    Command* command = inputHandler.handleInput('A');
    if(command)
    {
        command->execute(*player2);
    }
}