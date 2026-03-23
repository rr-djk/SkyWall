#pragma once

#include "../shared/Types.h"
#include "PlayerProcess.hpp"
#include <string>

/// @brief Advances a projectile's position by one tick.
/// @param p The projectile to update.
/// @param elapsed_seconds Time elapsed since the last tick, in seconds.
void updateProjectile(Projectile& p, float elapsed_seconds);

class GameEngine {
public:
    /// @brief Initializes the game state with tower positions.
    /// @param player_binary_path Path to the compiled player binary.
    explicit GameEngine(const std::string& player_binary_path);

    /// @brief Starts the game loop. Blocks until the game ends.
    void run();

private:
    GameState     state_;
    int           next_id_;
    bool          running_;
    PlayerProcess player_process_;

    /// @brief Applies updateProjectile to all active projectiles and interceptors.
    /// @param elapsed_seconds Time elapsed since the last tick, in seconds.
    void updateProjectilePositions(float elapsed_seconds);

    /// @brief Removes projectiles and interceptors that hit a wall or the player tower.
    void checkCollisions();

    /// @brief Fires a new projectile from the enemy tower.
    void fireEnemyProjectile();

    /// @brief Spawns an interceptor from the player tower aimed at the given target.
    /// @param target_x X coordinate of the interception point.
    /// @param target_y Y coordinate of the interception point.
    void spawnInterceptor(float target_x, float target_y);
};
