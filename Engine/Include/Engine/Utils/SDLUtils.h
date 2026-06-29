#pragma once
#include "Engine/Fwd/SDLFwd.h"

namespace SDLUtils {
    [[nodiscard]] bool InitSDLSystems();
    void ShutdownSDLSystems();
    UniqueWindow CreateSDLWindow(const char* title, int width, int height);
    UniqueGLContext CreateGLContext(SDL_Window* window);
    UniqueSurface LoadSurfaceFromImage(const std::string& imagePath);
    UniqueTTFFont LoadTTFFontFromFile(const std::string &filePath, int size);
}