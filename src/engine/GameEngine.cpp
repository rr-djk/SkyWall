#include "GameEngine.h"
#include "Board.h"
#include "Renderer.h"
#include "../shared/Constants.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <thread>

void updateProjectile(Projectile &p, float elapsed_seconds) {
  p.x += p.vel_x * elapsed_seconds;
  p.y += p.vel_y * elapsed_seconds;
}

GameEngine::GameEngine(const std::string& player_binary_path)
    : next_id_(0)
    , running_(false)
    , player_process_(player_binary_path)
{
  state_.player_tower_x = PLAYER_TOWER_X;
  state_.player_tower_y = TOWER_Y;
  state_.enemy_tower_x  = ENEMY_TOWER_X;
  state_.enemy_tower_y  = TOWER_Y;
}

void GameEngine::fireEnemyProjectile() {
  Projectile p;
  p.id    = next_id_++;
  p.x     = ENEMY_TOWER_X;
  p.y     = TOWER_Y;
  p.vel_x = PROJECTILE_SPEED;
  p.vel_y = 0.0f;
  state_.projectiles.push_back(p);
}

void GameEngine::spawnInterceptor(float target_x, float target_y) {
  const float origin_x = state_.player_tower_x;
  const float origin_y = state_.player_tower_y;

  const float dx = target_x - origin_x;
  const float dy = target_y - origin_y;
  const float distance = std::sqrt(dx * dx + dy * dy);

  // Évite une division par zéro si la cible est sur la tour.
  if (distance < 1e-4f)
    return;

  Projectile interceptor;
  interceptor.id    = next_id_++;
  interceptor.x     = origin_x;
  interceptor.y     = origin_y;
  interceptor.vel_x = (dx / distance) * INTERCEPTOR_SPEED;
  interceptor.vel_y = (dy / distance) * INTERCEPTOR_SPEED;
  state_.interceptors.push_back(interceptor);
}

void GameEngine::updateProjectilePositions(float elapsed_seconds) {
  for (auto &p : state_.projectiles)
    updateProjectile(p, elapsed_seconds);
  for (auto &p : state_.interceptors)
    updateProjectile(p, elapsed_seconds);
}

void GameEngine::checkCollisions() {
  auto& projs = state_.projectiles;
  projs.erase(std::remove_if(projs.begin(), projs.end(),
                             [](const Projectile& p) {
                               return Board::isOutOfBounds(p) ||
                                      Board::hitsPlayerTower(p);
                             }),
              projs.end());

  auto& intercepts = state_.interceptors;
  intercepts.erase(std::remove_if(intercepts.begin(), intercepts.end(),
                                  [](const Projectile& p) {
                                    return Board::isOutOfBounds(p);
                                  }),
                   intercepts.end());
}

void GameEngine::run() {
  running_ = true;
  player_process_.launch();
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

    player_process_.send_game_state(state_);

    const auto action = player_process_.receive_action();
    if (action.has_value() && action->fire)
      spawnInterceptor(action->target_x, action->target_y);

    std::this_thread::sleep_for(std::chrono::milliseconds(TICK_RATE_MS));
  }

  player_process_.terminate();
}
