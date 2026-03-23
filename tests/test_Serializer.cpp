#include "../src/ipc/Serializer.hpp"
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static Projectile make_projectile(int id, float x, float y, float vel_x, float vel_y) {
    Projectile p;
    p.id    = id;
    p.x     = x;
    p.y     = y;
    p.vel_x = vel_x;
    p.vel_y = vel_y;
    return p;
}

// ---------------------------------------------------------------------------
// GameState — round-trip
// ---------------------------------------------------------------------------

TEST(SerializerTest, GameState_RoundTrip_NoProjectiles) {
    GameState original;
    original.player_tower_x = 76.0f;
    original.player_tower_y = 12.0f;
    original.enemy_tower_x  = 2.0f;
    original.enemy_tower_y  = 12.0f;

    const GameState result = deserialize_game_state(serialize_game_state(original));

    EXPECT_EQ(result.projectiles.size(), 0u);
    EXPECT_FLOAT_EQ(result.player_tower_x, original.player_tower_x);
    EXPECT_FLOAT_EQ(result.player_tower_y, original.player_tower_y);
    EXPECT_FLOAT_EQ(result.enemy_tower_x,  original.enemy_tower_x);
    EXPECT_FLOAT_EQ(result.enemy_tower_y,  original.enemy_tower_y);
}

TEST(SerializerTest, GameState_RoundTrip_OneProjectile) {
    GameState original;
    original.player_tower_x = 76.0f;
    original.player_tower_y = 12.0f;
    original.enemy_tower_x  = 2.0f;
    original.enemy_tower_y  = 12.0f;
    original.projectiles.push_back(make_projectile(42, 15.5f, 7.3f, 20.0f, -1.5f));

    const GameState result = deserialize_game_state(serialize_game_state(original));

    ASSERT_EQ(result.projectiles.size(), 1u);
    EXPECT_EQ(result.projectiles[0].id,    42);
    EXPECT_FLOAT_EQ(result.projectiles[0].x,     15.5f);
    EXPECT_FLOAT_EQ(result.projectiles[0].y,     7.3f);
    EXPECT_FLOAT_EQ(result.projectiles[0].vel_x, 20.0f);
    EXPECT_FLOAT_EQ(result.projectiles[0].vel_y, -1.5f);
}

TEST(SerializerTest, GameState_RoundTrip_MultipleProjectiles) {
    GameState original;
    original.player_tower_x = 76.0f;
    original.player_tower_y = 12.0f;
    original.enemy_tower_x  = 2.0f;
    original.enemy_tower_y  = 12.0f;
    original.projectiles.push_back(make_projectile(0, 10.0f, 5.0f,  20.0f,  0.0f));
    original.projectiles.push_back(make_projectile(1, 30.0f, 8.0f,  20.0f,  2.0f));
    original.projectiles.push_back(make_projectile(2, 50.0f, 3.0f,  20.0f, -3.0f));

    const GameState result = deserialize_game_state(serialize_game_state(original));

    ASSERT_EQ(result.projectiles.size(), 3u);
    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(result.projectiles[i].id,    original.projectiles[i].id);
        EXPECT_FLOAT_EQ(result.projectiles[i].x,     original.projectiles[i].x);
        EXPECT_FLOAT_EQ(result.projectiles[i].y,     original.projectiles[i].y);
        EXPECT_FLOAT_EQ(result.projectiles[i].vel_x, original.projectiles[i].vel_x);
        EXPECT_FLOAT_EQ(result.projectiles[i].vel_y, original.projectiles[i].vel_y);
    }
}

// ---------------------------------------------------------------------------
// GameState — taille du buffer
// ---------------------------------------------------------------------------

TEST(SerializerTest, GameState_BufferSize_NoProjectiles) {
    GameState state;
    // 4 (count) + 4+4+4+4 (towers) = 20 bytes
    EXPECT_EQ(serialize_game_state(state).size(), 20u);
}

TEST(SerializerTest, GameState_BufferSize_OneProjectile) {
    GameState state;
    state.projectiles.push_back(make_projectile(0, 0, 0, 0, 0));
    // 4 (count) + 20 (projectile) + 16 (towers) = 40 bytes
    EXPECT_EQ(serialize_game_state(state).size(), 40u);
}

// ---------------------------------------------------------------------------
// Action — round-trip
// ---------------------------------------------------------------------------

TEST(SerializerTest, Action_RoundTrip_FireTrue) {
    Action original;
    original.fire     = true;
    original.target_x = 40.0f;
    original.target_y = 12.0f;

    const Action result = deserialize_action(serialize_action(original));

    EXPECT_TRUE(result.fire);
    EXPECT_FLOAT_EQ(result.target_x, original.target_x);
    EXPECT_FLOAT_EQ(result.target_y, original.target_y);
}

TEST(SerializerTest, Action_RoundTrip_FireFalse) {
    Action original;
    original.fire     = false;
    original.target_x = 0.0f;
    original.target_y = 0.0f;

    const Action result = deserialize_action(serialize_action(original));

    EXPECT_FALSE(result.fire);
    EXPECT_FLOAT_EQ(result.target_x, 0.0f);
    EXPECT_FLOAT_EQ(result.target_y, 0.0f);
}

// ---------------------------------------------------------------------------
// Action — taille du buffer
// ---------------------------------------------------------------------------

TEST(SerializerTest, Action_BufferSize) {
    Action action;
    // 1 (fire) + 4 (target_x) + 4 (target_y) = 9 bytes
    EXPECT_EQ(serialize_action(action).size(), 9u);
}
