#include "GameEngine.h"
#include "Board.h"
#include "Renderer.h"
#include "../shared/Constants.h"

#include <algorithm>
#include <chrono>
#include <thread>

void updateProjectile(Projectile &p, float elapsed_seconds) {
  p.x += p.vel_x * elapsed_seconds;
  p.y += p.vel_y * elapsed_seconds;
}

GameEngine::GameEngine() : next_id_(0), running_(false) {
  state_.player_tower_x = PLAYER_TOWER_X;
  state_.player_tower_y = TOWER_Y;
  state_.enemy_tower_x = ENEMY_TOWER_X;
  state_.enemy_tower_y = TOWER_Y;
}

void GameEngine::fireEnemyProjectile() {
  Projectile p;
  p.id = next_id_++;
  p.x = ENEMY_TOWER_X;
  p.y = TOWER_Y;
  p.vel_x = PROJECTILE_SPEED;
  p.vel_y = 0.0f;
  state_.projectiles.push_back(p);
}

void GameEngine::updateProjectilePositions(float elapsed_seconds) {
  for (auto &p : state_.projectiles)
    updateProjectile(p, elapsed_seconds);
}

void GameEngine::checkCollisions() {
  auto &projs = state_.projectiles;
  projs.erase(std::remove_if(projs.begin(), projs.end(),
                             [](const Projectile &p) {
                               return Board::isOutOfBounds(p) ||
                                      Board::hitsPlayerTower(p);
                             }),
              projs.end());
}

void GameEngine::run() {
  running_ = true;
  fireEnemyProjectile();

  using clock = std::chrono::steady_clock;
  auto last_tick = clock::now();

  while (running_) {
    auto now = clock::now();
    float elapsed_seconds =
        std::chrono::duration<float>(now - last_tick).count();
    last_tick = now;

    updateProjectilePositions(elapsed_seconds);
    checkCollisions();

    if (state_.projectiles.empty())
      fireEnemyProjectile();

    Renderer::draw(state_);

    std::this_thread::sleep_for(std::chrono::milliseconds(TICK_RATE_MS));
  }
}
