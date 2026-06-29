#pragma once
#include <memory>

using Uint8 = uint8_t;

struct SDL_Window;
struct SDLWindowDeleter {
    void operator()(SDL_Window*) const;
};

using UniqueWindow = std::unique_ptr<SDL_Window, SDLWindowDeleter>;

struct GLContextDeleter {
    void operator()(void* context) const;
};

using UniqueGLContext = std::unique_ptr<void, GLContextDeleter>;

struct SDL_Surface;
struct SDLSurfaceDeleter {
    void operator()(SDL_Surface* surface) const;
};

using UniqueSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDeleter>;

struct TTF_Font;
struct TTF_FontDeleter {
    void operator()(TTF_Font* font) const;
};

using UniqueTTFFont = std::unique_ptr<TTF_Font, TTF_FontDeleter>;