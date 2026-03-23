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
    // Ne tire pas si un intercepteur est déjà en vol.
    if (!state.interceptors.empty())
        return Action{0.0f, 0.0f, false};

    // Vise la position courante du premier projectile qui se dirige vers la tour.
    for (const auto& p : state.projectiles) {
        if (p.vel_x > 0.0f)
            return Action{p.x, p.y, true};
    }

    return Action{0.0f, 0.0f, false};
}
