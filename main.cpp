#include "Engine/Engine.h"
#include "Source/Game.h"

int main() {
    if (Engine engine(std::make_unique<Game>()); engine.Initialize()) {
        engine.RunLoop();
    }

    return 0;
}
