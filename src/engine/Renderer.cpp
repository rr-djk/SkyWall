#include "Renderer.h"
#include "../shared/Constants.h"

#include <ncurses.h>

namespace Renderer {

void init() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
}

void shutdown() {
    endwin();
}

void draw(const GameState& state) {
    clear();

    // Murs horizontaux
    for (int x = 0; x <= BOARD_WIDTH; ++x) {
        mvaddch(0,            x, '-');
        mvaddch(BOARD_HEIGHT, x, '-');
    }

    // Murs verticaux
    for (int y = 0; y <= BOARD_HEIGHT; ++y) {
        mvaddch(y, 0,           '|');
        mvaddch(y, BOARD_WIDTH, '|');
    }

    // Coins
    mvaddch(0,            0,           '+');
    mvaddch(0,            BOARD_WIDTH, '+');
    mvaddch(BOARD_HEIGHT, 0,           '+');
    mvaddch(BOARD_HEIGHT, BOARD_WIDTH, '+');

    // Tours
    mvprintw((int)state.enemy_tower_y,  (int)state.enemy_tower_x,  "[E]");
    mvprintw((int)state.player_tower_y, (int)state.player_tower_x, "[P]");

    // Projectiles
    for (const auto& p : state.projectiles)
        mvaddch((int)p.y, (int)p.x, 'o');

    refresh();
}

}
