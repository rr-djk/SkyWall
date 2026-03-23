#include "PlayerRunner.hpp"
#include "../ipc/PipeComm.hpp"

#include <cstdlib>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Player loop
// ---------------------------------------------------------------------------

void run_player_loop(int read_fd, int write_fd) {
    // Wrap the raw fds in a PipeComm configured for the player side.
    // PipeComm normally creates its own pipes, so we bypass construction
    // by providing fds directly via a dedicated factory.
    PipeComm comm(read_fd, write_fd);

    while (true) {
        GameState state;
        try {
            state = comm.read_game_state_blocking();
        } catch (const std::runtime_error&) {
            // Engine closed the pipe — exit cleanly.
            break;
        }

        const Action action = decide(state);

        try {
            comm.write_action(action);
        } catch (const std::runtime_error&) {
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return EXIT_FAILURE;
    }

    const int read_fd  = std::atoi(argv[1]);
    const int write_fd = std::atoi(argv[2]);

    run_player_loop(read_fd, write_fd);
    return EXIT_SUCCESS;
}
