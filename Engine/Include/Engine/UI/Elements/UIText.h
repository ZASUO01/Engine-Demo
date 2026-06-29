#pragma once
#include <string>
#include <unordered_map>
#include "UIElement.h"

class UniformsBlock;
class Texture;
namespace FontData {
    struct GlyphInfo;
    using GlyphTable = std::unordered_map<char, GlyphInfo>;
}

class UIText : public UIElement {
public:
    explicit UIText(const Vector2& offset = Vector2::Zero, float scale = 1.0f,
        float angle = 0.0f, int drawOrder = 100);

    void SetFont(class Font* font);
    void SetFontSize(int fontSize);
    void SetWrapLength(int wrapLength);
    void SetLineSpacing(float lineSpacing);
    void SetText(const std::string& text);
    void SetColor(const Vector3& color) { mColor = color; }

    void SendDrawCommand(Renderer* renderer) override;

private:
    Font* mFont;
    const FontData::GlyphTable* mGlyphMapCache;
    Texture* mAtlasTextureCache;

    int mFontSize;
    Vector3 mColor;
    std::string mText;
    int mWrapLength;
    float mLineSpacing;

    [[nodiscard]] const FontData::GlyphInfo* GetCharMapping(char c) const;
};