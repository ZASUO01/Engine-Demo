#include "MainCamera.h"

MainCamera::MainCamera(const int width, const int height):Camera("MainCamera") {
    mEye = Vector3{0.0f, 3.0f, 5.0f};

    mWidth = static_cast<float>(width);
    mHeight = static_cast<float>(height);

    Set3DView();
    Set3DProjection();
}