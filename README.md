# SkyWall

> Your fortress is under attack. Your only weapon: the algorithm you write.

SkyWall is a real-time terminal game where you implement the C++ interception
algorithm defending your aerial fortress against incoming threats.

No UI. No mouse. Just code.

---

## How it works

The engine handles everything: physics, projectiles, timing, rendering.
Your job is to implement a single function:

```cpp
Action decide(const GameState& state);
```

Called every tick, this function is your entire defense strategy.
Write it well — or watch your fortress fall.

---

## The challenge

Incoming projectiles won't wait for you. Your algorithm must:

- **Predict** where threats are heading
- **Intercept** them before they reach the fortress
- **Run fast** — decisions must be made in under 5ms

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
| 1 | Straight Line | Basic interception |
| 2 | Variable Speed | Recalculate on acceleration |
| 3 | Multi-Threat | Prioritize simultaneous projectiles |
| 4 | Smart Bombs | Dynamic replanning |
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
git clone https://github.com/yourname/skywall
cd skywall
make
./skywall
```

Open `src/player.cpp`, implement `decide()`, and launch.

---

## Requirements

- C++17
- CMake 3.15+
- A terminal
- Good instincts

---

## Contributing

SkyWall is a learning project. There are no wrong answers — only faster ones.

Optimize your `decide()`, experiment with new strategies, and share what you find.
If you cracked level 5 with a clever approach, open a PR. The best algorithms deserve to be seen.
