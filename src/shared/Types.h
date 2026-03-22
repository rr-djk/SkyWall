#pragma once

#include <vector>

struct Projectile {
    int   id;
    float x, y;
    float vel_x, vel_y;
};

struct GameState {
    std::vector<Projectile> projectiles;
    float player_tower_x, player_tower_y;
    float enemy_tower_x,  enemy_tower_y;
};

struct Action {
    float target_x, target_y;
    bool  fire;
};
