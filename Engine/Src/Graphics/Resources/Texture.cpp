#include "Engine/Graphics/Resources/Texture.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>

Texture::Texture() = default;

Texture::~Texture() {
    Destroy();
}

bool Texture::BuildFromSurface(const UniqueSurface &surface) {
    if (!surface) {
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    while (glGetError() != GL_NO_ERROR) {}

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // ensure pixel alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    const int pixelsPerLine = surface->pitch / surface->format->BytesPerPixel;
    glPixelStorei(GL_UNPACK_ROW_LENGTH, pixelsPerLine);

    // send data to the VRAM
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // restore to default alignment
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if (glGetError() != GL_NO_ERROR) {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
        mWidth = 0;
        mHeight = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool Texture::BuildEmpty(const int width, const int height) {
    if (width <= 0 || height <= 0) {
        return false;
    }

    if (mTextureID != 0) {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
    }

    mWidth = width;
    mHeight = height;

    while (glGetError() != GL_NO_ERROR) {}

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (glGetError() != GL_NO_ERROR) {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
        mWidth = 0;
        mHeight = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    return true;
}

bool Texture::BuildDepth(const int width, const int height) {
    if (width <= 0 || height <= 0) {
        return false;
    }

    if (mTextureID != 0) {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
    }

    mWidth = width;
    mHeight = height;

    while (glGetError() != GL_NO_ERROR) {}

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    if (glGetError() != GL_NO_ERROR) {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
        mWidth = 0;
        mHeight = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Texture::SetActive(const int index) const {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void Texture::Destroy() {
    if (mTextureID != 0) {
        glDeleteTextures(1, &mTextureID);
        mTextureID = 0;
    }
}

