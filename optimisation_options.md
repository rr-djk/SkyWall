# Optimisation Options

Pistes d'optimisation identifiées mais non implémentées, à considérer une fois le MVP fonctionnel.

---

## Board::isOutOfBounds — unsigned cast trick

**Fichier :** `src/engine/Board.cpp`

**Version actuelle :**
```cpp
bool isOutOfBounds(const Projectile& p) {
    return p.x <= 0 || p.x >= BOARD_WIDTH
        || p.y <= 0 || p.y >= BOARD_HEIGHT;
}
```

**Optimisation possible :**

Un cast `(unsigned)(int)` sur un float négatif produit un très grand entier, forcément supérieur à toute borne positive. Cela permet de fusionner les deux comparaisons par axe en une seule :

```cpp
bool isOutOfBounds(const Projectile& p) {
    return (unsigned)(int)p.x >= (unsigned)BOARD_WIDTH
        || (unsigned)(int)p.y >= (unsigned)BOARD_HEIGHT;
}
```

Réduit de 4 comparaisons à 2. Le cast `(int)` avant `(unsigned)` est nécessaire — caster un `float` négatif directement en `unsigned` est undefined behavior en C++.

**Limitation :** ne détecte pas `x == 0` comme hors limites sans ajustement explicite de la borne, ce qui nuit à la lisibilité.

---

## PipeComm — Gestion des erreurs sans exception

**Fichier :** `src/ipc/PipeComm.cpp`

**Situation actuelle :**
`PipeComm` lève des `std::runtime_error` sur les erreurs d'I/O (pipe rompu, écriture partielle, fd invalide). Dans un contexte temps réel, une exception non catchée dans la game loop fait crasher l'engine.

**Amélioration possible :**
Remplacer les exceptions par un type de retour d'erreur explicite, par exemple `std::expected<T, PipeError>` (C++23) ou un `enum class PipeError` retourné via `std::optional<std::pair<T, PipeError>>`. Le moteur peut alors décider de la politique : ignorer le tick raté, logger l'erreur, ou stopper proprement.

**Pourquoi ne pas le faire maintenant :**
Le jeu est mono-utilisateur local — un crash pipe signifie que le player process est mort, ce qui est une erreur fatale de toute façon. À revisiter si on ajoute un mode réseau ou un player process redémarrable.
