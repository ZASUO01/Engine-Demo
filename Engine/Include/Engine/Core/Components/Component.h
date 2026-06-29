#pragma once
#include "Engine/Math/Math.h"
#include "Engine/Config/ConfigFlags.h"

class Actor;
class Renderer;

class Component {
public:
    explicit Component(Actor *actor) : mActor(actor) {}
    virtual ~Component() = default;

    virtual void Update(float deltaTime) {}
    virtual void SendDrawCommand(Renderer* renderer, DrawMode mode) {}
    virtual void SendDebugDrawCommand(Renderer* renderer, DrawMode mode) {}

    [[nodiscard]] int GetUpdateOrder() const { return mUpdateOrder; }
    void SetUpdateOrder(const int updateOrder) { mUpdateOrder = updateOrder; }

    [[nodiscard]] int GetDrawOrder() const { return mDrawOrder; }
    void SetDrawOrder(const int drawOrder) { mDrawOrder = drawOrder; }

    void SetEnabled(const bool enabled) { mIsEnabled = enabled; }
    [[nodiscard]] bool IsEnabled() const { return mIsEnabled; }

    [[nodiscard]] Actor *GetActor() const { return mActor; }
private:
    int mUpdateOrder{100};
    int mDrawOrder{100};
    bool mIsEnabled{true};

    Actor *mActor;
};
