#pragma once

#include "../shared/Types.h"

/// @brief Advances a projectile's position by one tick.
/// @param p The projectile to update.
/// @param elapsed_seconds Time elapsed since the last tick, in seconds.
void updateProjectile(Projectile& p, float elapsed_seconds);

class GameEngine {
public:
    /// @brief Initializes the game state with tower positions.
    GameEngine();

    /// @brief Starts the game loop. Blocks until the game ends.
    void run();

private:
    GameState state_;
    int       next_id_;
    bool      running_;

    /// @brief Applies updateProjectile to all active projectiles.
    /// @param elapsed_seconds Time elapsed since the last tick, in seconds.
    void updateProjectilePositions(float elapsed_seconds);

    /// @brief Removes projectiles that hit a wall or the player tower.
    void checkCollisions();

    /// @brief Fires a new projectile from the enemy tower.
    void fireEnemyProjectile();
};
