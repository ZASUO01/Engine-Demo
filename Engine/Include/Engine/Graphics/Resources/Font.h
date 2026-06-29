#pragma once
#include <unordered_map>
#include <memory>
#include "Engine/Fwd/SDLFwd.h"

class Texture;
namespace FontData {
    struct GlyphInfo {
        float uMin, vMin;
        float uMax, vMax;

        int width, height;
        int bearingX, bearingY;
        int advance;
    };

    using GlyphTable = std::unordered_map<char, GlyphInfo>;
}

class Font {
public:
    Font();
    ~Font();

    // Load from a file
    bool Build(std::unordered_map<int, UniqueTTFFont>&& fonts);

    const FontData::GlyphTable* GetGlyphTable(int fontSize) const;
    Texture* GetAtlasTexture(int fontSize) const;

private:
    static constexpr int ATLAS_WIDTH = 512;
    static constexpr int ATLAS_HEIGHT = 512;

    std::unordered_map<int , std::unique_ptr<Texture>> mAtlasTexturesMap;
    std::unordered_map<int, std::unordered_map<char, FontData::GlyphInfo>> mGlyphsMap;

    void GenerateAtlas(TTF_Font* font, int size);
};
