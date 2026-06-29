#pragma once
#include "Camera.h"

class DefaultCamera : public Camera {
public:
    explicit DefaultCamera(float width, float height);
    ~DefaultCamera() override;
};
