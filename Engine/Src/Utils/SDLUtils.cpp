#include "Engine/Utils/SDLUtils.h"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

void SDLWindowDeleter::operator()(SDL_Window* window) const {
    if (window) {
        SDL_DestroyWindow(window);
    }
}

void GLContextDeleter::operator()(void* context) const {
    if (context) {
        SDL_GL_DeleteContext(context);
    }
}

void SDLSurfaceDeleter::operator()(SDL_Surface* surface) const {
    if (surface) {
        SDL_FreeSurface(surface);
    }
}

void TTF_FontDeleter::operator()(TTF_Font * font) const {
    if (font) {
        TTF_CloseFont(font);
    }
}

bool SDLUtils::InitSDLSystems() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) != 0) {
        SDL_Log("[SDL UTILS] Failed to initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (constexpr int imgFlags = IMG_INIT_PNG; !(IMG_Init(imgFlags) & imgFlags)){
        SDL_Log("[SDL UTILS] Unable to initialize SDL_image: %s", IMG_GetError());
        return false;
    }

    if (TTF_Init() != 0){
        SDL_Log("[SDL CONTROL] Failed to initialize SDL_ttf %s", TTF_GetError());
        return false;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    return true;
}

void SDLUtils::ShutdownSDLSystems() {
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

UniqueWindow SDLUtils::CreateSDLWindow(const char* title, const int width, const int height) {
    const auto window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("[SDL UTILS] Failed to create window: %s", SDL_GetError());
        return {nullptr};
    }

    return UniqueWindow(window);
}

UniqueGLContext SDLUtils::CreateGLContext(SDL_Window* window) {
    const auto context = SDL_GL_CreateContext(window);

    if (!context) {
        SDL_Log("[SDL UTILS] Failed to create OpenGL context: %s", SDL_GetError());
        return {nullptr};
    }

    return UniqueGLContext(context);
}

UniqueSurface SDLUtils::LoadSurfaceFromImage(const std::string &imagePath) {
    SDL_Surface* loaded = IMG_Load(imagePath.c_str());

    if (!loaded) {
        SDL_Log("[SDL UTILS] Failed to load image %s: %s", imagePath.c_str(), IMG_GetError());
        return {nullptr};
    }

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(loaded);

    if (!surface) {
        SDL_Log("[SDLUtils] Failed to convert surface format %s: %s", imagePath.c_str(), IMG_GetError());
        return {nullptr};
    }

    return UniqueSurface(surface);
}


UniqueTTFFont SDLUtils::LoadTTFFontFromFile(const std::string &filePath, const int size) {
    TTF_Font* font = TTF_OpenFont(filePath.c_str(), size);

    if (!font) {
        SDL_Log("[SDL UTILS] Failed to load font %s: %s", filePath.c_str(), TTF_GetError());
        return {nullptr};
    }

    return UniqueTTFFont(font);
}