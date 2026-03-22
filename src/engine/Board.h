#pragma once

#include "../shared/Types.h"
#include "../shared/Constants.h"

namespace Board {

    /// @brief Checks if a projectile has left the board.
    /// @param p The projectile to check.
    /// @return true if out of bounds, false otherwise.
    bool isOutOfBounds(const Projectile& p);

    /// @brief Checks if a projectile has reached the player tower.
    /// @param p The projectile to check.
    /// @return true if the projectile reached the player tower, false otherwise.
    /// @note Does not check y position — refined in later levels.
    bool hitsPlayerTower(const Projectile& p);

}
