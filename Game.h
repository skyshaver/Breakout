#pragma once

#include <array>
#include <vector>
#include <tuple>
#include <memory>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GlobalConstants.h"

enum class GameState 
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum class Direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

struct Collision
{
    bool isCollision;
    Direction direction;
    glm::vec2 diffVec;
};

class AudioEngine;
class BallObject;
class GameObject;
class GameLevel;
class ParticleGenerator;
class PostProcessor;
class PowerUpManager;
class SpriteRenderer;

class Game
{
public:

    Game(unsigned int width, unsigned int height);
    ~Game();
    
    void init();
    // game loop
    void processInput(float dt);
    void update(float dt);
    void render();
    void resetPlayer();
    void resetLevel();

    void doCollisionsExist();
    bool checkCollision(GameObject& one, GameObject& two) const; // axis-aligned bounding box
    //bool checkCollision(BallObject& one, GameObject& two) const; // axis-aligned bounding box on circlular object
    Collision checkCollision(BallObject& one, GameObject& two); 
    Direction vectorDirection(glm::vec2 target);

    // game state
    GameState state;
    std::array<bool, 1024> keys; // used to pass key state between game object and glfw
    unsigned int width, height;

    std::vector<GameLevel> levels;
    unsigned int level;

    std::unique_ptr<AudioEngine> audioEngine;
    std::unique_ptr<BallObject> ball;
    std::unique_ptr<GameObject> player;
    std::unique_ptr<SpriteRenderer> renderer;
    std::unique_ptr<ParticleGenerator> particles;
    std::unique_ptr<PostProcessor> effects;
    std::unique_ptr<PowerUpManager> powerUpManager;

private:
    glm::vec2 startPlayerPos{0.f};
    glm::vec2 startBallPos{0.f};
    float shaketime = 0.f;
    unsigned int playerLives = PLAYER_INITIAL_LIVES;
};

