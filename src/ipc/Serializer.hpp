#pragma once

#include "../shared/Types.h"
#include <cstdint>
#include <vector>

/// @brief Encodes a GameState into a compact binary buffer.
///
/// Format:
///   [4 bytes] projectile_count (uint32_t)
///   [20 bytes × N] per projectile:  id(4) x(4) y(4) vel_x(4) vel_y(4)
///   [4 bytes] interceptor_count (uint32_t)
///   [20 bytes × M] per interceptor: id(4) x(4) y(4) vel_x(4) vel_y(4)
///   [4 bytes] player_tower_x (float)
///   [4 bytes] player_tower_y (float)
///   [4 bytes] enemy_tower_x  (float)
///   [4 bytes] enemy_tower_y  (float)
///
/// @param state The game state to serialize.
/// @return A byte buffer representing the game state.
std::vector<uint8_t> serialize_game_state(const GameState& state);

/// @brief Decodes a binary buffer into a GameState.
/// @param buffer The byte buffer produced by serialize_game_state.
/// @return The reconstructed GameState.
/// @note Behavior is undefined if the buffer is malformed or truncated.
GameState deserialize_game_state(const std::vector<uint8_t>& buffer);

/// @brief Encodes an Action into a compact binary buffer.
///
/// Format:
///   [1 byte]  fire (uint8_t, 0 or 1)
///   [4 bytes] target_x (float)
///   [4 bytes] target_y (float)
///
/// @param action The action to serialize.
/// @return A byte buffer representing the action.
std::vector<uint8_t> serialize_action(const Action& action);

/// @brief Decodes a binary buffer into an Action.
/// @param buffer The byte buffer produced by serialize_action.
/// @return The reconstructed Action.
/// @note Behavior is undefined if the buffer is malformed or truncated.
Action deserialize_action(const std::vector<uint8_t>& buffer);
