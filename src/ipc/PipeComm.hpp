#pragma once

#include "../shared/Types.h"
#include <cstdint>
#include <optional>
#include <vector>

/// @brief Manages two anonymous Unix pipes between the engine and player processes.
///
/// Pipe layout:
///   engine_to_player[1] → engine_to_player[0]   (engine writes GameState, player reads)
///   player_to_engine[1] → player_to_engine[0]   (player writes Action,    engine reads)
///
/// Each message is framed as:
///   [4 bytes: payload_size (uint32_t)] [payload_size bytes: binary payload]
///
/// Usage:
///   1. Construct PipeComm in the engine before forking.
///   2. After fork: engine calls close_player_fds(), player calls close_engine_fds().
///   3. Engine uses write_game_state() / read_action_nonblocking().
///   4. Player uses read_game_state_blocking() / write_action().
class PipeComm {
public:
    /// @brief Creates both pipes. Throws std::runtime_error on failure.
    PipeComm();

    ~PipeComm();

    // Non-copyable
    PipeComm(const PipeComm&)            = delete;
    PipeComm& operator=(const PipeComm&) = delete;

    /// @brief Closes the file descriptors used by the player process.
    ///        Must be called in the engine process after fork().
    void close_player_fds();

    /// @brief Closes the file descriptors used by the engine process.
    ///        Must be called in the player process after fork().
    void close_engine_fds();

    /// @brief Returns the fd the player process should read GameState from.
    /// @return Read end of engine_to_player pipe.
    int player_read_fd() const;

    /// @brief Returns the fd the player process should write Action to.
    /// @return Write end of player_to_engine pipe.
    int player_write_fd() const;

    // -----------------------------------------------------------------------
    // Engine side
    // -----------------------------------------------------------------------

    /// @brief Serializes and writes a GameState into the engine-to-player pipe.
    /// @param state The game state to send to the player process.
    void write_game_state(const GameState& state);

    /// @brief Attempts to read an Action from the player-to-engine pipe without blocking.
    /// @return The Action if one was available this tick, or std::nullopt otherwise.
    std::optional<Action> read_action_nonblocking();

    // -----------------------------------------------------------------------
    // Player side
    // -----------------------------------------------------------------------

    /// @brief Blocks until a full GameState is received from the engine.
    /// @return The deserialized GameState.
    GameState read_game_state_blocking();

    /// @brief Serializes and writes an Action into the player-to-engine pipe.
    /// @param action The action decided by the player algorithm.
    void write_action(const Action& action);

private:
    int engine_to_player_[2];  ///< [0]=read (player), [1]=write (engine)
    int player_to_engine_[2];  ///< [0]=read (engine), [1]=write (player)

    /// @brief Writes a length-prefixed message to a file descriptor.
    /// @param fd      Destination file descriptor.
    /// @param payload The raw bytes to send.
    void write_message(int fd, const std::vector<uint8_t>& payload);

    /// @brief Reads a full length-prefixed message from a file descriptor (blocking).
    /// @param fd Source file descriptor.
    /// @return The raw payload bytes.
    std::vector<uint8_t> read_message_blocking(int fd);
};
