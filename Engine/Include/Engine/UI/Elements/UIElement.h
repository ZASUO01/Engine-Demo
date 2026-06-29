#pragma once
#include "Engine/Math/Math.h"

class Renderer;

class UIElement {
public:
    explicit UIElement(const Vector2 &offset = Vector2::Zero,
        const Vector2 &size = Vector2::One, float scale = 1.0f, float angle = 0.0f, int drawOrder = 100);
    virtual ~UIElement();

    // Getters and Setters
    [[nodiscard]] const Vector2& GetOffset() const { return mOffset; }
    void SetOffset(const Vector2& offset);

    [[nodiscard]] const Vector2& GetSize() const { return mSize; }
    void SetSize(const Vector2& size);

    [[nodiscard]] float GetScale() const { return mScale; }
    void SetScale(float scale);

    [[nodiscard]] float GetAngle() const { return mAngle; }
    void SetAngle(float angle);

    const Matrix4& GetTransform();

    [[nodiscard]] bool IsVisible() const { return mIsVisible; }
    void SetVisible(const bool visible) { mIsVisible = visible; }

    [[nodiscard]] int GetDrawOrder() const { return mDrawOrder; }
    void SetDrawOrder(const int drawOrder) { mDrawOrder = drawOrder; }

    [[nodiscard]] class Shader* GetShader() const { return mShader; }
    void SetShader(Shader* shader) { mShader = shader; }

    // Defines how to send draw commands to the renderer
    virtual void SendDrawCommand(Renderer* renderer) = 0;
private:
    Vector2 mOffset;
    Vector2 mSize;
    float mScale;
    float mAngle;

    Matrix4 mTransform;
    bool mIsTransformDirty;

    bool mIsVisible;
    int mDrawOrder;

    Shader* mShader;
};