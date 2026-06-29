#include <utility>
#include "Engine/Core/Cameras/Camera.h"

Camera::Camera(std::string name)
:mName(std::move(name))
{}

Camera::~Camera() = default;

void Camera::Set3DView() {
    mViewMatrix = Matrix4::CreateLookAt(mEye, mTarget, mUp);
    mIsViewDirty = true;
}

void Camera::Set3DProjection() {
    mProjectionMatrix = Matrix4::CreatePerspectiveFOV(mFov, mWidth , mHeight, mNear, mFar);
    mIsProjectionDirty = true;
}
