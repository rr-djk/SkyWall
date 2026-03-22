#include "GameEngine.h"
#include "Renderer.h"

int main() {
    Renderer::init();

    GameEngine engine;
    engine.run();

    Renderer::shutdown();
    return 0;
}
