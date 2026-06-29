#include "Engine/Core/Cameras/DefaultCamera.h"

DefaultCamera::DefaultCamera(const float width, const float height) : Camera("Default") {
    mEye = Vector3(3.0f, 3.0f, 3.0f);

    mWidth = width;
    mHeight = height;


    Set3DView();
    Set3DProjection();
}

DefaultCamera::~DefaultCamera()  = default;