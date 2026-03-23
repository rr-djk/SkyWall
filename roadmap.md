# SkyWall — Roadmap

## Description (CORE DU PROJET)

SkyWall est un **jeu éducatif en C++ dans le terminal** destiné à des développeurs intermédiaires souhaitant apprendre les bases de l'IA, de la simulation temps réel et du low latency.

Le jeu se déroule sur un **repère de coordonnées 2D délimité par des murs**. Deux tours statiques s'affrontent face à face sur l'axe X, aux extrémités du terrain.

- Une tour ennemie (contrôlée par le moteur) lance des projectiles vers la tour du joueur
- Une tour défensive (contrôlée par le joueur via du code) doit intercepter ces projectiles

Tout projectile — ennemi ou intercepteur — qui touche un mur est neutralisé.

### Objectif

Le joueur implémente un algorithme capable de :
1. **Détecter** les projectiles ennemis se dirigeant vers sa tour
2. **Calculer** le point d'interception
3. **Tirer** un intercepteur au bon endroit, au bon moment

---

## Terrain de jeu

```
(0,max) +---------------------------------+ (max,max)
        |                                 |
        |  [ENEMY]   💣 -->    [PLAYER]  |
        |  (x=0)               (x=max)   |
        |                                 |
(0,0)   +---------------------------------+ (max,0)
```

- Le terrain est un rectangle fermé (murs sur les 4 côtés)
- Les tours sont statiques, positionnées aux extrémités de l'axe X
- Les projectiles se déplacent selon une vélocité `(vx, vy)`

---

## Architecture — Pipe inter-process (option B)

Le moteur et l'algorithme du joueur tournent dans **deux processus séparés** qui communiquent via des pipes Unix.

```
┌─────────────────────────────────────┐
│              ENGINE                 │
│                                     │
│  game loop (tick)                   │
│    ├── update positions             │
│    ├── draw screen (ncurses)        │
│    ├── write GameState → pipe_in    │
│    └── read Action ← pipe_out       │
└─────────────────────────────────────┘
          │ pipe          │ pipe
┌─────────────────────────────────────┐
│          PLAYER PROCESS             │
│    read GameState                   │
│    → decide()                       │
│    → write Action                   │
└─────────────────────────────────────┘
```

Le moteur n'est jamais bloqué par le joueur : si la réponse n'arrive pas dans le tick courant, il continue d'animer. Un algo trop lent rate sa fenêtre de tir.

### Structures de données

```cpp
struct Projectile {
    int   id;
    float x, y;   // position actuelle
    float vx, vy; // vélocité
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

### Rendu

- Interface interactive **ncurses** dans le terminal
- Chaque tick : mise à jour des positions + redessinage de l'écran
- Panneau de métriques en temps réel (latence, interceptions, score)

---

## Game Loop

```
tick:
  1. update projectile positions (x += vx * dt)
  2. check wall collisions → neutralize
  3. check interceptions → score
  4. draw screen (ncurses)
  5. write GameState → pipe
  6. (non-blocking) read Action ← pipe
  7. if action.fire → spawn interceptor
```

---

## Métriques

- Temps de réponse (ms)
- Interceptions réussies / total
- Précision (%)
- Nombre de recalculs

---

## Progression des niveaux

### Niveau 1 — Trajectoire simple
- Projectile en ligne droite (vy = 0)
- Objectif : interception basique

### Niveau 2 — Vitesse variable
- Accélération des projectiles
- Recalcul nécessaire en cours de vol

### Niveau 3 — Multiples projectiles
- Plusieurs projectiles simultanés
- Priorisation : lequel intercepter en premier ?

### Niveau 4 — Projectiles intelligents
- Changement de trajectoire en vol
- Replanning dynamique

### Niveau 5 — Contraintes temps réel
- Limite stricte de latence
- Pénalité si dépassement de la fenêtre de tir

---

## Composants techniques clés

### Rendu terminal
- ncurses pour l'interface interactive
- Rafraîchissement partiel de l'écran à chaque tick

### Communication IPC
- Pipes Unix entre engine et player process
- Sérialisation/désérialisation du GameState
- Lecture non-bloquante côté moteur

### Low latency
- Mesure du temps de réponse de `decide()`
- Pénalités si dépassement du budget temps

### Algorithmes (côté joueur)
- Détection des projectiles menaçants (vx > 0 vers la tour)
- Prédiction de position future : `x' = x + vx * t`
- Calcul du point d'interception
- Priorisation multi-menaces

### Mémoire
- Structures cache-friendly
- Éviter les copies inutiles dans la boucle de jeu

---

## Étapes de développement

### ✅ Phase 1 — MVP (terminée)
- Terrain délimité, deux tours statiques
- 1 projectile ennemi en ligne droite
- Rendu ncurses basique
- Tests unitaires Board (Google Test)
- CMakeLists.txt avec cible `make unit_tests`

### ✅ Phase 2 — IPC & Communication (terminée)
- ✅ Sérialisation binaire GameState / Action (`src/ipc/Serializer`)
- ✅ Pipes anonymes Unix avec framing et lecture non-bloquante (`src/ipc/PipeComm`)
- ✅ Cycle de vie du process joueur via fork + exec (`src/engine/PlayerProcess`)
- ✅ Boucle joueur et stub `decide()` (`src/player/`)
- ✅ CI GitHub Actions sur push .cpp/.hpp/.h
- ✅ Intégration dans GameEngine (send/receive par tick, spawn intercepteur)

### Phase 3 — Interception
- Algorithme de détection et calcul de trajectoire
- Logique de tir du joueur

### Phase 4 — Système de niveaux
- Progression et validation
- Difficulté croissante

### Phase 5 — Performance & Feedback
- Timer et métriques
- Hints après échec

---

## Extensions possibles

- Mode sandbox (terrain libre, expérimentation)
- Replay système
- Leaderboard
- Stress test (100+ projectiles simultanés)
- Visualisation des trajectoires prédites
- **Agrandissement du board** — le board actuel (80x24) n'occupe qu'un quart
  d'un écran 23 pouces. ncurses permet de récupérer dynamiquement la taille
  du terminal (`getmaxyx()`) pour adapter le board à la résolution disponible,
  ce qui offrirait un terrain de jeu plus large et des trajectoires plus complexes.

---

## Objectif final

Créer un environnement où un développeur apprend à :

- penser en temps réel
- optimiser du code C++
- écrire des algorithmes efficaces
- comprendre la latence et l'IPC

> SkyWall n'est pas juste un jeu — c'est un simulateur d'algorithmes temps réel orienté performance.
