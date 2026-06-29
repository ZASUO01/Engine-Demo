#pragma once
#include "Engine/Math/Math.h"

class Renderer;

class GlobalLight {
public:
    explicit GlobalLight(float radius, float height, float moveSpeed = 1.0f);
    ~GlobalLight();

    void ProcessInput(int key);
    void Update(float deltaTime);
    void SetRenderLight(Renderer* renderer);

    void SetLightEnabled(const bool enable) { mLightEnabled = enable; }
private:
    float mRadius{0.0f};
    float mHeight{0.0f};

    Vector3 mLightPosition{Vector3::Zero};
    float mMoveSpeed;
    float mInputIntent{0.0f};
    float mLightOrbitAngle{0.0f};

    bool mLightEnabled;
    bool mLightPositionDirty{true};
};
