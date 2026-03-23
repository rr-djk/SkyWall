#include "Serializer.hpp"

#include <cstring>

// ---------------------------------------------------------------------------
// Helpers internes
// ---------------------------------------------------------------------------

/// @brief Appends the raw bytes of a value to a buffer.
/// @param buffer Destination buffer.
/// @param value  Value to append (copied byte-by-byte).
template <typename T>
static void append_bytes(std::vector<uint8_t>& buffer, const T& value) {
    const uint8_t* start = reinterpret_cast<const uint8_t*>(&value);
    buffer.insert(buffer.end(), start, start + sizeof(T));
}

/// @brief Reads a value from a buffer at a given offset and advances that offset.
/// @param buffer Source buffer.
/// @param offset Current read position, incremented by sizeof(T) after the read.
/// @return The value decoded from the buffer.
template <typename T>
static T read_bytes(const std::vector<uint8_t>& buffer, size_t& offset) {
    T value;
    std::memcpy(&value, buffer.data() + offset, sizeof(T));
    offset += sizeof(T);
    return value;
}

// ---------------------------------------------------------------------------
// GameState
// ---------------------------------------------------------------------------

std::vector<uint8_t> serialize_game_state(const GameState& state) {
    std::vector<uint8_t> buffer;

    const uint32_t projectile_count = static_cast<uint32_t>(state.projectiles.size());
    append_bytes(buffer, projectile_count);

    for (const Projectile& p : state.projectiles) {
        append_bytes(buffer, p.id);
        append_bytes(buffer, p.x);
        append_bytes(buffer, p.y);
        append_bytes(buffer, p.vel_x);
        append_bytes(buffer, p.vel_y);
    }

    append_bytes(buffer, state.player_tower_x);
    append_bytes(buffer, state.player_tower_y);
    append_bytes(buffer, state.enemy_tower_x);
    append_bytes(buffer, state.enemy_tower_y);

    return buffer;
}

GameState deserialize_game_state(const std::vector<uint8_t>& buffer) {
    GameState state;
    size_t offset = 0;

    const uint32_t projectile_count = read_bytes<uint32_t>(buffer, offset);
    state.projectiles.resize(projectile_count);

    for (uint32_t i = 0; i < projectile_count; ++i) {
        state.projectiles[i].id    = read_bytes<int>(buffer, offset);
        state.projectiles[i].x     = read_bytes<float>(buffer, offset);
        state.projectiles[i].y     = read_bytes<float>(buffer, offset);
        state.projectiles[i].vel_x = read_bytes<float>(buffer, offset);
        state.projectiles[i].vel_y = read_bytes<float>(buffer, offset);
    }

    state.player_tower_x = read_bytes<float>(buffer, offset);
    state.player_tower_y = read_bytes<float>(buffer, offset);
    state.enemy_tower_x  = read_bytes<float>(buffer, offset);
    state.enemy_tower_y  = read_bytes<float>(buffer, offset);

    return state;
}

// ---------------------------------------------------------------------------
// Action
// ---------------------------------------------------------------------------

std::vector<uint8_t> serialize_action(const Action& action) {
    std::vector<uint8_t> buffer;

    const uint8_t fire_byte = action.fire ? 1 : 0;
    append_bytes(buffer, fire_byte);
    append_bytes(buffer, action.target_x);
    append_bytes(buffer, action.target_y);

    return buffer;
}

Action deserialize_action(const std::vector<uint8_t>& buffer) {
    Action action;
    size_t offset = 0;

    const uint8_t fire_byte = read_bytes<uint8_t>(buffer, offset);
    action.fire     = (fire_byte != 0);
    action.target_x = read_bytes<float>(buffer, offset);
    action.target_y = read_bytes<float>(buffer, offset);

    return action;
}
