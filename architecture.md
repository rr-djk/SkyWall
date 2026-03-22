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
│   │   ├── Renderer.cpp/h      # ncurses — affichage terminal
│   │   ├── main.cpp            # Entry point
│   │   └── LevelManager.cpp/h  # Gestion des niveaux (Phase 3)
│   │
│   ├── ipc/                    # Communication inter-processus (Phase 2)
│   │   ├── PipeComm.cpp/h      # Pipes Unix, lecture non-bloquante
│   │   └── Serializer.cpp/h    # Sérialisation GameState ↔ bytes
│   │
│   ├── player/                 # Interface joueur (Phase 2)
│   │   ├── PlayerRunner.cpp/h  # Entry point du process joueur
│   │   └── player.cpp          # ← L'utilisateur n'édite que ce fichier
│   │
│   └── shared/                 # Types partagés engine + player
│       ├── Types.h             # Projectile, GameState, Action
│       └── Constants.h         # Dimensions terrain, vitesses, tick rate
│
├── include/skywall/            # Headers publics exposés au joueur
│   └── skywall.h               # API publique : decide(), GameState, Action
│
├── levels/                     # Définitions des niveaux (Phase 3)
├── tests/                      # Tests unitaires (Google Test)
├── optimisation_options.md     # Pistes d'optimisation identifiées
└── CMakeLists.txt
```

---

## Composants

### shared/

**Types partagés** entre engine et player process.

| Fichier | Responsabilité |
|---------|----------------|
| `Types.h` | `Projectile`, `GameState`, `Action` |
| `Constants.h` | Dimensions terrain, vitesses, tick rate |

```cpp
struct Projectile {
    int   id;
    float x, y;         // position courante
    float vel_x, vel_y; // vélocité (unités/seconde)
};

struct GameState {
    std::vector<Projectile> projectiles;
    float player_tower_x, player_tower_y;
    float enemy_tower_x,  enemy_tower_y;
};

struct Action {
    float target_x, target_y; // coordonnée cible du tir
    bool  fire;
};
```

### engine/

**Moteur de jeu** — processus principal qui anime la partie.

| Fichier | Responsabilité |
|---------|----------------|
| `GameEngine.cpp/h` | Boucle de jeu (tick), orchestration |
| `Board.cpp/h` | Terrain, murs, limites, détection collisions |
| `Renderer.cpp/h` | Affichage ncurses, double buffering |
| `main.cpp` | Init ncurses, instancie GameEngine, shutdown |

---

## Cycle du jeu

### Démarrage

```
main()
  ├── Renderer::init()        — initialise ncurses
  ├── GameEngine engine       — initialise GameState avec positions des tours
  ├── engine.run()            — démarre la boucle, bloquant jusqu'à la fin
  └── Renderer::shutdown()    — restaure le terminal
```

### Un tick (~16ms à 60fps)

```
GameEngine::run() — boucle while(running_)
  ├── calcule elapsed_seconds depuis le dernier tick
  ├── updateProjectilePositions(elapsed_seconds)
  │     └── pour chaque projectile : x += vel_x * elapsed_seconds
  │                                  y += vel_y * elapsed_seconds
  ├── checkCollisions()
  │     └── retire les projectiles hors limites (Board::isOutOfBounds)
  │         ou ayant atteint la tour joueur    (Board::hitsPlayerTower)
  ├── fireEnemyProjectile() si plus aucun projectile actif
  ├── Renderer::draw(state_)
  │     ├── clear()
  │     ├── dessine les murs (+, -, |)
  │     ├── dessine [E] et [P] aux positions des tours
  │     ├── dessine o à la position de chaque projectile
  │     └── refresh()
  └── sleep(TICK_RATE_MS)     — régule à ~60fps
```

### Communication IPC (Phase 2)

```
tick Engine → sérialise GameState → écrit dans pipe_in du player
           ← lit Action depuis pipe_out (non-bloquant)
           → si action.fire : tire un intercepteur

tick Player → lit GameState depuis pipe_in
           → decide() calcule la réponse
           → écrit Action dans pipe_out
```

Le moteur n'attend jamais la réponse du joueur. Si elle n'arrive pas dans le tick courant, la fenêtre de tir est perdue.

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
[20 bytes: Projectile * N]   ← id(4) + x(4) + y(4) + vel_x(4) + vel_y(4)
[8 bytes: player_tower_x, player_tower_y]
[8 bytes: enemy_tower_x,  enemy_tower_y]
```

---

## Niveaux

| Niveau | Description |
|--------|-------------|
| 1 | Trajectoire droite, vel_y = 0 |
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
