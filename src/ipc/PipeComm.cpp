#include "PipeComm.hpp"
#include "Serializer.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

PipeComm::PipeComm() {
    if (pipe(engine_to_player_) == -1) {
        throw std::runtime_error(std::string("pipe() failed: ") + std::strerror(errno));
    }
    if (pipe(player_to_engine_) == -1) {
        close(engine_to_player_[0]);
        close(engine_to_player_[1]);
        throw std::runtime_error(std::string("pipe() failed: ") + std::strerror(errno));
    }
}

PipeComm::PipeComm(int read_fd, int write_fd)
    : engine_to_player_{read_fd, -1}
    , player_to_engine_{-1, write_fd}
{}

PipeComm::~PipeComm() {
    for (int fd : {engine_to_player_[0], engine_to_player_[1],
                   player_to_engine_[0], player_to_engine_[1]}) {
        if (fd != -1) {
            close(fd);
        }
    }
}

// ---------------------------------------------------------------------------
// Post-fork fd management
// ---------------------------------------------------------------------------

void PipeComm::close_player_fds() {
    close(engine_to_player_[0]);
    engine_to_player_[0] = -1;
    close(player_to_engine_[1]);
    player_to_engine_[1] = -1;
}

void PipeComm::close_engine_fds() {
    close(engine_to_player_[1]);
    engine_to_player_[1] = -1;
    close(player_to_engine_[0]);
    player_to_engine_[0] = -1;
}

int PipeComm::player_read_fd() const {
    return engine_to_player_[0];
}

int PipeComm::player_write_fd() const {
    return player_to_engine_[1];
}

// ---------------------------------------------------------------------------
// Internal framing
// ---------------------------------------------------------------------------

void PipeComm::write_message(int fd, const std::vector<uint8_t>& payload) {
    const uint32_t size = static_cast<uint32_t>(payload.size());

    const ssize_t header_written = write(fd, &size, sizeof(size));
    if (header_written != sizeof(size)) {
        throw std::runtime_error("write_message: failed to write length header");
    }

    size_t bytes_written = 0;
    while (bytes_written < payload.size()) {
        const ssize_t n = write(fd, payload.data() + bytes_written,
                                payload.size() - bytes_written);
        if (n <= 0) {
            throw std::runtime_error("write_message: failed to write payload");
        }
        bytes_written += static_cast<size_t>(n);
    }
}

std::vector<uint8_t> PipeComm::read_message_blocking(int fd) {
    uint32_t size    = 0;
    size_t bytes_read = 0;

    while (bytes_read < sizeof(size)) {
        const ssize_t n = read(fd, reinterpret_cast<uint8_t*>(&size) + bytes_read,
                               sizeof(size) - bytes_read);
        if (n <= 0) {
            throw std::runtime_error("read_message_blocking: failed to read length header");
        }
        bytes_read += static_cast<size_t>(n);
    }

    std::vector<uint8_t> payload(size);
    bytes_read = 0;

    while (bytes_read < size) {
        const ssize_t n = read(fd, payload.data() + bytes_read, size - bytes_read);
        if (n <= 0) {
            throw std::runtime_error("read_message_blocking: failed to read payload");
        }
        bytes_read += static_cast<size_t>(n);
    }

    return payload;
}

// ---------------------------------------------------------------------------
// Engine side
// ---------------------------------------------------------------------------

void PipeComm::write_game_state(const GameState& state) {
    write_message(engine_to_player_[1], serialize_game_state(state));
}

std::optional<Action> PipeComm::read_action_nonblocking() {
    const int fd = player_to_engine_[0];

    const int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    uint32_t size    = 0;
    const ssize_t n  = read(fd, &size, sizeof(size));

    if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        fcntl(fd, F_SETFL, flags);
        return std::nullopt;
    }

    if (n != sizeof(size)) {
        fcntl(fd, F_SETFL, flags);
        throw std::runtime_error("read_action_nonblocking: partial or failed header read");
    }

    // Header received — restore blocking mode to read the payload reliably.
    fcntl(fd, F_SETFL, flags);

    std::vector<uint8_t> payload(size);
    size_t bytes_read = 0;
    while (bytes_read < size) {
        const ssize_t m = read(fd, payload.data() + bytes_read, size - bytes_read);
        if (m <= 0) {
            throw std::runtime_error("read_action_nonblocking: failed to read payload");
        }
        bytes_read += static_cast<size_t>(m);
    }

    return deserialize_action(payload);
}

// ---------------------------------------------------------------------------
// Player side
// ---------------------------------------------------------------------------

GameState PipeComm::read_game_state_blocking() {
    return deserialize_game_state(read_message_blocking(engine_to_player_[0]));
}

void PipeComm::write_action(const Action& action) {
    write_message(player_to_engine_[1], serialize_action(action));
}
