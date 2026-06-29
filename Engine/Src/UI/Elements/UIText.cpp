#include "Engine/UI/Elements/UIText.h"
#include "Engine/Graphics/Resources/Font.h"
#include "Engine/Graphics/Resources/Texture.h"
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Graphics/Renderer/RenderUtils.h"
#include "Engine/Graphics/Renderer/Renderer.h"

UIText::UIText(const Vector2 &offset, const float scale, const float angle, const int drawOrder)
:UIElement(offset, Vector2::One, scale, angle, drawOrder)
,mFont(nullptr)
,mGlyphMapCache(nullptr)
,mAtlasTextureCache(nullptr)
,mFontSize(40)
,mColor(Color::Green)
,mText({})
,mWrapLength(600)
,mLineSpacing(1.2f) {
}

void UIText::SetFont(Font *font) {
    if (font) {
        mFont = font;
        mGlyphMapCache = mFont->GetGlyphTable(mFontSize);
        mAtlasTextureCache = mFont->GetAtlasTexture(mFontSize);
    }
}

void UIText::SetFontSize(const int fontSize) {
    if (fontSize == mFontSize) {
        return;
    }

    mFontSize = fontSize;
    mGlyphMapCache = mFont->GetGlyphTable(mFontSize);
    mAtlasTextureCache = mFont->GetAtlasTexture(mFontSize);
}

void UIText::SetText(const std::string& text) {
    if (text == mText) {
        return;
    }

    mText = text;
}

void UIText::SetWrapLength(const int wrapLength) {
    mWrapLength = Math::Clamp(wrapLength, 600, 1024);
}

void UIText::SetLineSpacing(const float lineSpacing) {
    mLineSpacing = Math::Clamp(lineSpacing, 1.2f, 1.5f);
}

void UIText::SendDrawCommand(Renderer *renderer) {
    if (mText.empty() || !mFont) {
        return;
    }

    const auto texture = mAtlasTextureCache;
    if (!texture) {
        return;
    }

    const auto shader = GetShader();
    const auto shaderID = shader ?  shader->GetID() : 0;
    const auto textureID = texture->GetID();
    const uint64_t key = RenderUtils::GenerateRenderKey(shaderID, 0, textureID, GetDrawOrder());

    const Vector2 originalOffset = GetOffset();

    float currentX = 0.0f;
    float currentY = 0.0f;
    const float lineHeight = static_cast<float>(mFontSize) * mLineSpacing;

    for (const char c : mText) {
        // Break line for \n
        if (c == '\n') {
            currentX = 0.0f;
            currentY += lineHeight;
            continue;
        }

        // Get char information
        const auto glyph = GetCharMapping(c);
        if (!glyph) {
            continue;
        }

        // break line case this char breaks the wrap length
        if (currentX + static_cast<float>(glyph->advance) > static_cast<float>(mWrapLength)) {
            currentX = 0.0f;
            currentY += lineHeight;
        }

        const auto width = static_cast<float>(glyph->width);
        const auto height = static_cast<float>(glyph->height);

        // Start position in pixels
        const float xPos = currentX + static_cast<float>(glyph->bearingX);
        const float yPos = currentY;

        // The size of this element as the char width and height
        SetSize(Vector2{width, height});

        // Start position added to offset
        SetOffset(originalOffset + Vector2{xPos, yPos});

        RenderUtils::SpriteCommand cmd = {
            key,
            shaderID,
            textureID,
            shader,
            texture,
            GetTransform(),
            Vector4{glyph->uMin, glyph->vMin, glyph->uMax - glyph->uMin, glyph->vMax - glyph->vMin},
            mColor
        };

        renderer->PushSpritesCommand(cmd);
        currentX += static_cast<float>(glyph->advance);
    }
    SetOffset(originalOffset);
}

const FontData::GlyphInfo *UIText::GetCharMapping(const char c) const {
    if (!mGlyphMapCache) {
        return nullptr;
    }

    if (const auto it = mGlyphMapCache->find(c); it != mGlyphMapCache->end()) {
        return &it->second;
    }

    if (const auto it = mGlyphMapCache->find(' '); it != mGlyphMapCache->end()) {
        return &it->second;
    }

    return nullptr;
}