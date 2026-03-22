#pragma once

#include "../shared/Types.h"

namespace Renderer {

    /// @brief Initializes the ncurses context. Must be called once before draw().
    void init();

    /// @brief Shuts down the ncurses context. Must be called before exit.
    void shutdown();

    /// @brief Draws the current game state: walls, towers, and projectiles.
    /// @param state The current game state to render.
    void draw(const GameState& state);

}
