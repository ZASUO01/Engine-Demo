#pragma once
#include <string>
#include "Engine/Math/Math.h"

class Camera {
public:
    explicit Camera(std::string name);
    virtual ~Camera();

protected:
    Vector3 mEye{Vector3::One};
    Vector3 mTarget{Vector3::Zero};
    Vector3 mUp{Vector3::UnitY};

    float mWidth{0.0f};
    float mHeight{0.0f};
    float mFov{45.0f};
    float mNear{0.1f};
    float mFar{1000.0f};

    void Set3DProjection();
    void Set3DView();

    virtual void ProcessInput(int key) {}
    virtual void Update(float deltaTime) {}

private:
    Matrix4 mViewMatrix{Matrix4::Identity};
    Matrix4 mProjectionMatrix{Matrix4::Identity};
    bool mIsViewDirty{true};
    bool mIsProjectionDirty{true};
    bool mIsActive{true};
    std::string mName;

    friend class CamerasManager;
};
