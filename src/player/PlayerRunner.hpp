#pragma once

#include "../shared/Types.h"

/// @brief Runs the player process main loop.
///
/// Reads GameState frames from read_fd, calls decide(), and writes
/// the resulting Action back to write_fd. Exits cleanly when the
/// engine closes the pipe.
///
/// @param read_fd  File descriptor to read GameState from (engine → player pipe).
/// @param write_fd File descriptor to write Action to (player → engine pipe).
void run_player_loop(int read_fd, int write_fd);

/// @brief Computes the player's response to the current game state.
///
/// This is the only function the player needs to implement.
/// The stub in player.cpp returns a no-fire Action by default.
///
/// @param state The current game state sent by the engine.
/// @return The action to perform this tick (fire or hold).
Action decide(const GameState& state);
