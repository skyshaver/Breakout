#pragma once

#include <glm/glm.hpp>

constexpr unsigned int SCREEN_WIDTH = 800;
constexpr unsigned int SCREEN_HEIGHT = 600;

constexpr glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
constexpr float PLAYER_VELOCITY = 500.0f;
constexpr unsigned int PLAYER_INITIAL_LIVES = 3;
constexpr glm::vec2 PLAYER_START_POSITION = glm::vec2(SCREEN_WIDTH / 2.0f - PLAYER_SIZE.x / 2.0f, SCREEN_HEIGHT - PLAYER_SIZE.y);

constexpr glm::vec2 BALL_INITIAL_VELOCITY(200.0f, -350.0f);
constexpr float BALL_RADIUS = 12.5f;
constexpr glm::vec2 BALL_START_POSITION = PLAYER_START_POSITION + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
constexpr int BALL_DEATH_LINE = 560;

constexpr int PARTICLES_INITIAL_COUNT = 500;
constexpr int PARTICLES_NEW_PER_FRAME = 2;

constexpr float POWERUP_GOOD_CHANCE = .013f; // 0.013 for gamplay, higher for testing
constexpr float POWERUP_BAD_CHANCE = .066f; // 0.066 for gameplay, higher for testing