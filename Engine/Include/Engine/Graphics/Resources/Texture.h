#pragma once
#include "Engine/Fwd/SDLFwd.h"

class Texture {
public:
    Texture();
    ~Texture();

    [[nodiscard]] bool BuildFromSurface(const UniqueSurface &surface);
    [[nodiscard]] bool BuildEmpty(int width, int height);
    [[nodiscard]] bool BuildDepth(int width, int height);

    void SetActive(int index = 0) const;
    [[nodiscard]] bool IsValid() const  { return mTextureID != 0; }

    [[nodiscard]] int GetWidth() const { return mWidth; }
    [[nodiscard]] int GetHeight() const { return mHeight; }

    [[nodiscard]] unsigned int GetID() const { return mTextureID; }
private:
    void Destroy();

    unsigned int mTextureID{0};
    int mWidth{0};
    int mHeight{0};
};