#include "PlayerRunner.hpp"

// ---------------------------------------------------------------------------
// decide() — la seule fonction que le joueur doit implémenter.
//
// Elle est appelée à chaque tick par run_player_loop() avec l'état
// courant du jeu. La valeur retournée est envoyée immédiatement au moteur.
//
// Contrainte temps réel : une réponse trop lente rate la fenêtre de tir.
// ---------------------------------------------------------------------------

Action decide(const GameState& state) {
    (void)state;
    return Action{0.0f, 0.0f, false};
}
