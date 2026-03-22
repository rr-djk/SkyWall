#pragma once

constexpr int BOARD_WIDTH  = 80;
constexpr int BOARD_HEIGHT = 24;

constexpr float ENEMY_TOWER_X  = 2.0f;
constexpr float PLAYER_TOWER_X = 77.0f;
constexpr float TOWER_Y        = BOARD_HEIGHT / 2.0f;

// Vitesse initiale des projectiles ennemis.
// Migrera vers la config des niveaux quand la difficulté évoluera.
constexpr float PROJECTILE_SPEED = 20.0f;

// Durée d'un tick en millisecondes (~60fps).
constexpr int TICK_RATE_MS = 16;
