#pragma once
#include "ColliderComponent.h"

class BoxColliderComponent : public ColliderComponent {
public:
    BoxColliderComponent(Actor *actor, PhysicsSystem* physicsSystem, const Vector3& extents, bool isTrigger = false);

    [[nodiscard]] const Vector3& GetExtents() const { return mExtents;}

    void SendDebugDrawCommand(Renderer *renderer, DrawMode mode) override;
private:
    Vector3 mExtents;
};