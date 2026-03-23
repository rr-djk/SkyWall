#include "../src/player/PlayerRunner.hpp"
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
// decide() — validité de la valeur de retour
// ---------------------------------------------------------------------------

TEST(PlayerTest, Decide_EmptyState_ReturnsValidAction) {
    GameState state;
    state.player_tower_x = 76.0f;
    state.player_tower_y = 12.0f;
    state.enemy_tower_x  = 2.0f;
    state.enemy_tower_y  = 12.0f;

    // Should not throw and return a well-formed Action.
    Action action;
    EXPECT_NO_THROW(action = decide(state));

    // fire=false is a valid no-op response.
    EXPECT_FALSE(action.fire);
}

TEST(PlayerTest, Decide_WithProjectiles_DoesNotCrash) {
    GameState state;
    state.player_tower_x = 76.0f;
    state.player_tower_y = 12.0f;
    state.enemy_tower_x  = 2.0f;
    state.enemy_tower_y  = 12.0f;
    state.projectiles.push_back(make_projectile(0, 30.0f, 12.0f, 20.0f,  0.0f));
    state.projectiles.push_back(make_projectile(1, 15.0f,  8.0f, 20.0f, -1.5f));

    EXPECT_NO_THROW(decide(state));
}

TEST(PlayerTest, Decide_CalledMultipleTimes_DoesNotCrash) {
    GameState state;
    state.player_tower_x = 76.0f;
    state.player_tower_y = 12.0f;
    state.enemy_tower_x  = 2.0f;
    state.enemy_tower_y  = 12.0f;

    for (int i = 0; i < 100; ++i) {
        state.projectiles.clear();
        state.projectiles.push_back(make_projectile(i, static_cast<float>(i), 12.0f, 20.0f, 0.0f));
        EXPECT_NO_THROW(decide(state));
    }
}
