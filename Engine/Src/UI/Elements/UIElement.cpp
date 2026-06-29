#include "Engine/UI/Elements/UIElement.h"

UIElement::UIElement(const Vector2 &offset, const Vector2& size, const float scale, const float angle, const int drawOrder)
:mOffset(offset)
,mSize(size)
,mScale(scale)
,mAngle(angle)
,mTransform(Matrix4::Identity)
,mIsTransformDirty(true),
mIsVisible(true)
,mDrawOrder(drawOrder)
,mShader(nullptr){}

UIElement::~UIElement() = default;

void UIElement::SetAngle(const float angle) {
    mAngle = angle;
    mIsTransformDirty = true;
}

void UIElement::SetScale(const float scale) {
    mScale = scale;
    mIsTransformDirty = true;
}

void UIElement::SetSize(const Vector2 &size) {
    mSize = size;
    mIsTransformDirty = true;
}

void UIElement::SetOffset(const Vector2 &offset) {
    mOffset = offset;
    mIsTransformDirty = true;
}

const Matrix4& UIElement::GetTransform() {
    if (mIsTransformDirty) {
        mIsTransformDirty = false;
        mTransform = Matrix4::CreateScale(mSize.x * mScale, mSize.y * mScale, 1.0f) *
                        Matrix4::CreateRotationZ(mAngle) *
                            Matrix4::CreateTranslation(Vector3(mOffset.x, mOffset.y, 0.0f));

        return mTransform;
    }

    return mTransform;
}