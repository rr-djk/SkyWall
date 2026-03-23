#include "PlayerProcess.hpp"

#include <csignal>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

PlayerProcess::PlayerProcess(const std::string& player_binary_path)
    : binary_path_(player_binary_path)
    , pid_(-1)
{}

void PlayerProcess::launch() {
    pid_ = fork();

    if (pid_ == -1) {
        throw std::runtime_error("PlayerProcess::launch: fork() failed");
    }

    if (pid_ == 0) {
        // Child process — becomes the player binary.
        comm_.close_engine_fds();

        const std::string read_fd_str  = std::to_string(comm_.player_read_fd());
        const std::string write_fd_str = std::to_string(comm_.player_write_fd());

        execl(binary_path_.c_str(),
              binary_path_.c_str(),
              read_fd_str.c_str(),
              write_fd_str.c_str(),
              nullptr);

        // execl only returns on failure.
        std::exit(EXIT_FAILURE);
    }

    // Parent process — engine side.
    comm_.close_player_fds();
}

void PlayerProcess::terminate() {
    if (pid_ <= 0) {
        return;
    }

    kill(pid_, SIGTERM);
    waitpid(pid_, nullptr, 0);
    pid_ = -1;
}

bool PlayerProcess::is_alive() const {
    if (pid_ <= 0) {
        return false;
    }

    const int result = waitpid(pid_, nullptr, WNOHANG);
    return result == 0;
}

void PlayerProcess::send_game_state(const GameState& state) {
    comm_.write_game_state(state);
}

std::optional<Action> PlayerProcess::receive_action() {
    return comm_.read_action_nonblocking();
}
