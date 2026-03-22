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
