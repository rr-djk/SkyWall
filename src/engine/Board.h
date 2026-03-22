#pragma once

#include "../shared/Types.h"
#include "../shared/Constants.h"

namespace Board {
    bool isOutOfBounds(const Projectile& p);
    bool hitsPlayerTower(const Projectile& p);
}
