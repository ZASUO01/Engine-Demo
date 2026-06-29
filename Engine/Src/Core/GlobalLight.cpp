#include "Engine/Core/GlobalLight.h"
#include <SDL2/SDL_scancode.h>
#include "Engine/Graphics/Renderer/Renderer.h"

GlobalLight::GlobalLight(const float radius, const float height, const float moveSpeed)
:mRadius(radius), mHeight(height), mMoveSpeed(moveSpeed), mLightEnabled(true) {
    mLightPosition.x = mRadius;
    mLightPosition.z = 0.0f;
    mLightPosition.y = mHeight;
}

GlobalLight::~GlobalLight() = default;

void GlobalLight::ProcessInput(const int key) {
    mInputIntent = 0.0f;

    if (key == SDL_SCANCODE_O) {
        mInputIntent += 1.0f;
    }

    if (key == SDL_SCANCODE_P) {
        mInputIntent -= 1.0f;
    }

}

void GlobalLight::Update(const float deltaTime) {
    if (!mLightEnabled) {
        return;
    }

    if (mInputIntent == 0.0f) {
        return;
    }

    mLightOrbitAngle += mInputIntent * mMoveSpeed * deltaTime;
    mInputIntent = 0.0f;
    if (mLightOrbitAngle >= Math::TwoPi) {
        mLightOrbitAngle -= Math::TwoPi;
    }

    if (mLightOrbitAngle <= 0) {
        mLightOrbitAngle += Math::TwoPi;
    }

    mLightPosition.x = mRadius * Math::Cos(mLightOrbitAngle);
    mLightPosition.z = mRadius * Math::Sin(mLightOrbitAngle);
    mLightPositionDirty = true;
}

void GlobalLight::SetRenderLight(Renderer *renderer) {
    if (!mLightEnabled || !renderer || !mLightPositionDirty) {
        return;
    }

    mLightPositionDirty = false;
    renderer->SetLightPosition(mLightPosition);
}
