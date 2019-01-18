#include <mbed.h>
#include "Game.h"

Game game;

int main() {
    game.init();
    while (true) {
        game.step();
        wait(constants::TIMEOUT_WHILE_LOOP);
    }
}