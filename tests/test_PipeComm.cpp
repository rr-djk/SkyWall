#include "../src/ipc/PipeComm.hpp"
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static GameState make_game_state() {
    GameState state;
    state.player_tower_x = 76.0f;
    state.player_tower_y = 12.0f;
    state.enemy_tower_x  = 2.0f;
    state.enemy_tower_y  = 12.0f;

    Projectile p;
    p.id    = 7;
    p.x     = 30.0f;
    p.y     = 8.5f;
    p.vel_x = 20.0f;
    p.vel_y = -1.0f;
    state.projectiles.push_back(p);

    return state;
}

static Action make_action(bool fire, float tx, float ty) {
    Action a;
    a.fire     = fire;
    a.target_x = tx;
    a.target_y = ty;
    return a;
}

// ---------------------------------------------------------------------------
// GameState round-trip via pipe (same process, sequential)
// ---------------------------------------------------------------------------

TEST(PipeCommTest, GameState_RoundTrip) {
    PipeComm comm;

    const GameState sent = make_game_state();
    comm.write_game_state(sent);

    const GameState received = comm.read_game_state_blocking();

    ASSERT_EQ(received.projectiles.size(), 1u);
    EXPECT_EQ(received.projectiles[0].id,      7);
    EXPECT_FLOAT_EQ(received.projectiles[0].x,     30.0f);
    EXPECT_FLOAT_EQ(received.projectiles[0].y,     8.5f);
    EXPECT_FLOAT_EQ(received.projectiles[0].vel_x, 20.0f);
    EXPECT_FLOAT_EQ(received.projectiles[0].vel_y, -1.0f);
    EXPECT_FLOAT_EQ(received.player_tower_x, 76.0f);
    EXPECT_FLOAT_EQ(received.enemy_tower_x,  2.0f);
}

// ---------------------------------------------------------------------------
// Action round-trip via pipe (same process, sequential)
// ---------------------------------------------------------------------------

TEST(PipeCommTest, Action_RoundTrip_FireTrue) {
    PipeComm comm;

    const Action sent = make_action(true, 45.0f, 10.0f);
    comm.write_action(sent);

    const std::optional<Action> received = comm.read_action_nonblocking();

    ASSERT_TRUE(received.has_value());
    EXPECT_TRUE(received->fire);
    EXPECT_FLOAT_EQ(received->target_x, 45.0f);
    EXPECT_FLOAT_EQ(received->target_y, 10.0f);
}

TEST(PipeCommTest, Action_RoundTrip_FireFalse) {
    PipeComm comm;

    const Action sent = make_action(false, 0.0f, 0.0f);
    comm.write_action(sent);

    const std::optional<Action> received = comm.read_action_nonblocking();

    ASSERT_TRUE(received.has_value());
    EXPECT_FALSE(received->fire);
}

// ---------------------------------------------------------------------------
// Non-blocking read returns nullopt when no data is available
// ---------------------------------------------------------------------------

TEST(PipeCommTest, ReadAction_ReturnsNullopt_WhenNothingWritten) {
    PipeComm comm;

    const std::optional<Action> received = comm.read_action_nonblocking();

    EXPECT_FALSE(received.has_value());
}

// ---------------------------------------------------------------------------
// Multiple messages in sequence
// ---------------------------------------------------------------------------

TEST(PipeCommTest, MultipleActions_ReceivedInOrder) {
    PipeComm comm;

    comm.write_action(make_action(true,  10.0f, 5.0f));
    comm.write_action(make_action(false, 20.0f, 8.0f));

    const auto first  = comm.read_action_nonblocking();
    const auto second = comm.read_action_nonblocking();

    ASSERT_TRUE(first.has_value());
    EXPECT_TRUE(first->fire);
    EXPECT_FLOAT_EQ(first->target_x, 10.0f);

    ASSERT_TRUE(second.has_value());
    EXPECT_FALSE(second->fire);
    EXPECT_FLOAT_EQ(second->target_x, 20.0f);
}
