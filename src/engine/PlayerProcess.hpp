#pragma once

#include "../ipc/PipeComm.hpp"
#include "../shared/Types.h"
#include <optional>
#include <string>
#include <sys/types.h>

/// @brief Manages the lifecycle of the player process and its IPC channel.
///
/// Forks the engine process and exec's the player binary, passing the pipe
/// file descriptors as command-line arguments so the player can communicate
/// back with the engine.
///
/// Usage:
///   PlayerProcess player("./skywall_player");
///   player.launch();
///   // in each tick:
///   player.send_game_state(state);
///   auto action = player.receive_action();  // non-blocking
///   // at shutdown:
///   player.terminate();
class PlayerProcess {
public:
    /// @brief Prepares the player process manager.
    /// @param player_binary_path Path to the compiled player binary.
    explicit PlayerProcess(const std::string& player_binary_path);

    /// @brief Forks the engine and exec's the player binary.
    ///        The player binary receives two arguments: read_fd and write_fd.
    /// @note  Must be called exactly once before any send/receive.
    void launch();

    /// @brief Sends SIGTERM to the player process and waits for it to exit.
    /// @note  Safe to call even if the process has already exited.
    void terminate();

    /// @brief Checks whether the player process is still running.
    /// @return true if the process is alive, false if it has exited or was never launched.
    bool is_alive() const;

    /// @brief Serializes and sends a GameState to the player process.
    /// @param state The current game state.
    void send_game_state(const GameState& state);

    /// @brief Attempts to read an Action from the player process without blocking.
    /// @return The Action if the player responded this tick, or std::nullopt.
    std::optional<Action> receive_action();

private:
    std::string binary_path_;
    pid_t       pid_;
    PipeComm    comm_;
};
