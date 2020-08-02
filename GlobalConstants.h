#pragma once

#include <glm/glm.hpp>

constexpr glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
constexpr float PLAYER_VELOCITY(500.0f);
constexpr glm::vec2 INITIAL_BALL_VELOCITY(200.0f, -350.0f);
constexpr float BALL_RADIUS = 12.5f;
constexpr int INITIAL_PARTICLES = 500;
constexpr int NEW_PARTICLES_PER_FRAME = 2;
constexpr float POWERUP_GOOD_CHANCE = .013f; // 0.013 for gamplay, higher for testing
constexpr float POWERUP_BAD_CHANCE = .066f; // 0.066 for gameplay, higher for testing