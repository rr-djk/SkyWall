#include "Board.h"

namespace Board {

bool isOutOfBounds(const Projectile& p) {
    return p.x <= 0 || p.x >= BOARD_WIDTH
        || p.y <= 0 || p.y >= BOARD_HEIGHT;
}

bool hitsPlayerTower(const Projectile& p) {
    return p.x >= PLAYER_TOWER_X;
}

}
