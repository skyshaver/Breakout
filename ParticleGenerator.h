#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"
#include "GameObject.h"


// Represents a single particle and its state
struct Particle 
{
    glm::vec2 position, velocity;
    glm::vec4 color;
    float     life;

    Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
    void debugprint() { std::cout << "particle " << this->position.x << ',' << this->position.y << '\n'; }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // update all particles
    void update(float dt, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // render all particles
    void draw();
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    Shader shader;
    Texture2D texture;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

