# SkyWall

> Your fortress is under attack. Your only weapon: the algorithm you write.

SkyWall is a real-time terminal game where you implement the C++ interception
algorithm defending your aerial fortress against incoming threats.

No UI. No mouse. Just code.

---

## How it works

Two static towers face each other across a bounded 2D field. The enemy tower
launches projectiles toward your fortress. Your job: detect the threats,
calculate the intercept point, and fire before it's too late.

The engine handles physics, rendering, and timing. Your job is to implement
a single function:

```cpp
Action decide(const GameState& state);
```

Your process receives the game state through a Unix pipe every tick and writes
back a firing action. The engine never waits for you — a slow algorithm misses
its firing window.

---

## The field

```
(0,max) +---------------------------------+ (max,max)
        |                                 |
        |  [ENEMY]   💣 -->    [PLAYER]   |
        |  (x=0)               (x=max)    |
        |                                 |
(0,0)   +---------------------------------+ (max,0)
```

Any projectile — enemy or interceptor — that hits a wall is neutralized.
Only projectiles heading toward your tower need to be intercepted.

---

## Architecture

The engine and your algorithm run as **two separate processes** communicating
via Unix pipes:

```
Engine  ──── GameState ────►  Your process
Engine  ◄─── Action ─────────  Your process
```

The engine updates positions, draws the screen, and writes the game state each
tick — without blocking on your response. Your process reads the state,
computes a firing solution, and writes back an action.

---

## The data

```cpp
struct Projectile {
    int   id;
    float x, y;       // current position
    float vel_x, vel_y; // velocity
};

struct GameState {
    std::vector<Projectile> projectiles;   // enemy projectiles
    std::vector<Projectile> interceptors;  // your interceptors in flight
    float player_tower_x, player_tower_y;
    float enemy_tower_x,  enemy_tower_y;
};

struct Action {
    bool  fire;
    float target_x, target_y;
};
```

Use `vel_x` and `vel_y` to determine which projectiles are heading toward you
and predict where they will be.

---

## The challenge

Your algorithm must:

- **Detect** which projectiles are a threat
- **Predict** their future position: `x' = x + vx * t`
- **Intercept** them before they reach your tower
- **Run fast** — you have one tick to respond

Every millisecond counts. Every allocation matters.

---

## What you will learn

- **Low-latency C++ optimization** — write code that makes decisions in microseconds, avoid allocations, think in cache lines
- **Trajectory prediction & interception** — implement the math to anticipate where a threat will be, not where it is
- **Memory-efficient coding** — design cache-friendly structures, eliminate copies, do more with less

---

## Levels

| # | Name | Challenge |
|---|------|-----------|
| 1 | Straight Line | Basic interception, projectiles travel in a straight line |
| 2 | Variable Speed | Projectiles accelerate — recalculate mid-flight |
| 3 | Multi-Threat | Multiple simultaneous projectiles — prioritize |
| 4 | Smart Bombs | Projectiles change trajectory — replan dynamically |
| 5 | Zero Tolerance | Strict latency limits — no mercy |

---

## Metrics

After each round, the engine reports:

- Response time (ms)
- Interceptions: X / Y
- Accuracy (%)
- Recomputations

---

## Getting started

```bash
git clone https://github.com/rr-djk/SkyWall
cd SkyWall
mkdir build && cd build
cmake ..
make
```

This produces two binaries in `build/`:

- `skywall` — the game engine
- `skywall_player` — your algorithm process

The engine launches `skywall_player` automatically when you start a game.
Both binaries must be in the same directory.

**Implement your algorithm** in `src/player/player.cpp`, then rebuild and run:

```bash
# from build/
make
./skywall
```

The engine sends you a `GameState` every tick via a Unix pipe and reads your
`Action` response without blocking. If your algorithm doesn't respond in time,
the tick passes without a shot.

---

## Requirements

- C++17
- CMake 3.15+
- ncurses
- A terminal
- Good instincts

---

## Contributing

SkyWall is a learning project. There are no wrong answers — only faster ones.

Optimize your `decide()`, experiment with new strategies, and share what you find.
If you cracked level 5 with a clever approach, open a PR. The best algorithms deserve to be seen.
