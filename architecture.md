# Architecture SkyWall

## Vue d'ensemble

SkyWall est un jeu éducatif C++ en terminal où le joueur implémente un algorithme d'interception face à un moteur de jeu. Les deux parties tournent dans des **processus séparés** communiquant via pipes Unix.

```
┌─────────────────────────────────────┐
│              ENGINE                 │
│  game loop (tick)                   │
│    ├── update projectile positions  │
│    ├── detect collisions            │
│    ├── draw screen (ncurses)        │
│    ├── write GameState → pipe       │
│    └── read Action ← pipe           │
└─────────────────────────────────────┘
                  │ pipe
┌─────────────────────────────────────┐
│          PLAYER PROCESS             │
│    read GameState                   │
│    → decide()                       │
│    → write Action                   │
└─────────────────────────────────────┘
```

---

## Structure du projet

```
SkyWall/
├── src/
│   ├── engine/                 # Moteur de jeu
│   │   ├── GameEngine.cpp/h    # Boucle principale, orchestration
│   │   ├── Board.cpp/h         # Terrain, murs, limites, collisions
│   │   ├── Projectile.cpp/h    # Gestion des projectiles
│   │   ├── Tower.cpp/h         # Tours (enemy + player)
│   │   ├── Renderer.cpp/h      # ncurses - affichage terminal
│   │   └── LevelManager.cpp/h  # Gestion des niveaux
│   │
│   ├── ipc/                    # Communication inter-processus
│   │   ├── PipeComm.cpp/h      # Pipes Unix, lecture non-bloquante
│   │   └── Serializer.cpp/h    # Sérialisation GameState ↔ bytes
│   │
│   ├── player/                 # Interface joueur
│   │   ├── PlayerRunner.cpp/h  # Entry point du process joueur
│   │   └── player.cpp          # ← L'utilisateur n'édite que ce fichier
│   │
│   └── shared/                 # Types partagés engine + player
│       ├── Types.h             # Projectile, GameState, Action
│       ├── Constants.h         # Dimensions terrain, vitesses max
│       └── Metrics.cpp/h       # Latence, score, précision
│
├── include/skywall/            # Headers publics exposés au joueur
│   └── skywall.h               # API publique : decide(), GameState, Action
│
├── levels/                     # Définitions des niveaux (config)
│   ├── level_1.json
│   ├── level_2.json
│   └── ...
│
├── tests/
└── CMakeLists.txt
```

---

## Composants

### shared/

**Types partagés** entre engine et player process.

| Fichier | Responsabilité |
|---------|----------------|
| `Types.h` | `Projectile`, `GameState`, `Action` |
| `Constants.h` | Dimensions terrain, vitesses max |
| `Metrics.cpp/h` | Latence, score, précision |

```cpp
struct Projectile {
    int   id;
    float x, y;      // position
    float vx, vy;    // vélocité
};

struct GameState {
    std::vector<Projectile> projectiles;
    float player_tower_x, player_tower_y;
    float enemy_tower_x,  enemy_tower_y;
};

struct Action {
    bool  fire;
    float target_x, target_y;
};
```

### engine/

**Moteur de jeu** — processus principal qui anime la partie.

| Fichier | Responsabilité |
|---------|----------------|
| `GameEngine.cpp/h` | Boucle de jeu (tick), orchestration |
| `Board.cpp/h` | Terrain, murs, limites, détection collisions |
| `Projectile.cpp/h` | Création, mouvement, cycle de vie |
| `Tower.cpp/h` | Position des tours, logique de tir |
| `Renderer.cpp/h` | Affichage ncurses, panneau métriques |
| `LevelManager.cpp/h` | Chargement et progression des niveaux |

**Game loop (tick) :**
```
1. update positions (x += vx * dt)
2. check wall collisions → neutralize  (Board)
3. check interceptions → score         (Board)
4. draw screen (ncurses)               (Renderer)
5. write GameState → pipe              (PipeComm)
6. (non-blocking) read Action ← pipe  (PipeComm)
7. if action.fire → spawn interceptor  (Tower)
```

### ipc/

**Communication inter-processus.**

| Fichier | Responsabilité |
|---------|----------------|
| `PipeComm.cpp/h` | Écriture/lecture pipes, non-bloquant |
| `Serializer.cpp/h` | Conversion GameState ↔ bytes |

### player/

**Interface exposée au joueur.**

```cpp
namespace skywall {
    // Called each tick with current game state
    // Must return in < 1ms for level 5+
    Action decide(const GameState& state);
}
```

---

## Communication IPC

### Protocole

1. Engine écrit `GameState` sérialisé dans `pipe_in` du player
2. Player lit, calcule, écrit `Action` dans `pipe_out`
3. Lecture **non-bloquante** côté engine — si timeout, pas de tir ce tick

### Sérialisation

Format binaire compact pour minimiser la latence :

```
[4 bytes: projectile_count]
[16 bytes: Projectile * N]
[8 bytes: player_tower_x, player_tower_y]
[8 bytes: enemy_tower_x, enemy_tower_y]
```

---

## Niveaux

| Niveau | Description |
|--------|-------------|
| 1 | Trajectoire droite, vy = 0 |
| 2 | Vitesse variable, recalcul nécessaire |
| 3 | Multiples projectiles, priorisation |
| 4 | Projectiles à trajectoire variable |
| 5 | Latence stricte, pénalité temps réel |

---

## Extensions futures

- Mode sandbox
- Système de replay
- Leaderboard
- Stress test (100+ projectiles)
- Visualisation trajectoires prédites
