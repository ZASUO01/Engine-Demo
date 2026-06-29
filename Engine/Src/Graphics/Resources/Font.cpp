#include  "Engine/Graphics/Resources/Font.h"
#include "Engine/Graphics/Resources/Texture.h"
#include "Engine/Utils/SDLUtils.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <vector>

static constexpr SDL_Color COLOR_WHITE = { 255, 255, 255, 255 };

Font::Font() = default;

Font::~Font() = default;

bool Font::Build(std::unordered_map<int, UniqueTTFFont>&& fonts) {
   if (fonts.empty()) {
       SDL_Log("[FONT] Fonts map empty");
       return false;
   }

    for (auto& [size, font]: fonts) {
        if (!font) {
            SDL_Log("[FONT] Font of size %d not found", size);
            return false;
        }

        GenerateAtlas(font.get(), size);
    }

    return true;
}

const FontData::GlyphTable *Font::GetGlyphTable(const int fontSize) const {
    if (const auto it = mGlyphsMap.find(fontSize); it != mGlyphsMap.end()) {
        return &it->second;
    }
    return nullptr;
}

Texture *Font::GetAtlasTexture(const int fontSize) const {
    if (const auto it = mAtlasTexturesMap.find(fontSize); it != mAtlasTexturesMap.end() ) {
        return it->second.get();
    }
    return nullptr;
}


void Font::GenerateAtlas(TTF_Font* font, const int size) {
    SDL_Surface* rawAtlas = SDL_CreateRGBSurfaceWithFormat(0, ATLAS_WIDTH, ATLAS_HEIGHT, 32, SDL_PIXELFORMAT_RGBA32);
    const auto atlasSurface = UniqueSurface(rawAtlas);

    SDL_FillRect(atlasSurface.get(), nullptr, SDL_MapRGBA(atlasSurface->format, 0, 0, 0, 0));

    int currentX = 0;
    int currentY = 0;
    int maxRowHeight = 0;

    // Map all ASCII chars
    for (char c = 32; c <= 126; ++c) {
        SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(font, c, COLOR_WHITE);
        if (!glyphSurface) {
            continue;
        }

        int minX, maxX, minY, maxY, advance;
        TTF_GlyphMetrics(font, c, &minX, &maxX, &minY, &maxY, &advance);

        if (currentX + glyphSurface->w >= ATLAS_WIDTH) {
            currentX = 0;
            currentY += maxRowHeight + 2;
            maxRowHeight = 0;
        }

        SDL_Rect destRect = { currentX, currentY, glyphSurface->w, glyphSurface->h };
        SDL_SetSurfaceBlendMode(glyphSurface, SDL_BLENDMODE_NONE);
        SDL_BlitSurface(glyphSurface, nullptr, atlasSurface.get(), &destRect);

        FontData::GlyphInfo glyph{};
        glyph.width = glyphSurface->w;
        glyph.height = glyphSurface->h;
        glyph.bearingX = minX;
        glyph.bearingY = TTF_FontAscent(font) - maxY;
        glyph.advance = advance;

        glyph.uMin = static_cast<float>(currentX) / static_cast<float>(ATLAS_WIDTH);
        glyph.vMin = static_cast<float>(currentY) / static_cast<float>(ATLAS_HEIGHT);
        glyph.uMax = static_cast<float>(currentX + glyph.width) / static_cast<float>(ATLAS_WIDTH);
        glyph.vMax = static_cast<float>(currentY + glyph.height) / static_cast<float>(ATLAS_HEIGHT);

        mGlyphsMap[size][c] = glyph;

        currentX += glyphSurface->w + 2;
        maxRowHeight = std::max(maxRowHeight, glyphSurface->h);

        SDL_FreeSurface(glyphSurface);
    }

    mAtlasTexturesMap[size] = std::make_unique<Texture>();

    if (!mAtlasTexturesMap[size]->BuildFromSurface(atlasSurface)) {
        mAtlasTexturesMap[size].reset();
    }
}