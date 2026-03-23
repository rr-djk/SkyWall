#include "GameEngine.h"
#include "Renderer.h"

int main() {
    Renderer::init();

    GameEngine engine("./skywall_player");
    engine.run();

    Renderer::shutdown();
    return 0;
}
