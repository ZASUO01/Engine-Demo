#pragma once
#include "ColliderComponent.h"

class PlaneColliderComponent : public ColliderComponent {
public:
    explicit PlaneColliderComponent(Actor* actor, PhysicsSystem* physicsSystem, bool isTrigger = false);

    [[nodiscard]] float GetDistanceToOrigin() const { return mDistanceToOrigin; }
    [[nodiscard]] const Vector3& GetNormal() const { return mNormal; }

    void Update(float deltaTime) override;
    void SendDebugDrawCommand(Renderer *renderer, DrawMode mode) override;
private:
    void CalculateFromOwner();

    Matrix4 mTransform{Matrix4::Identity};
    bool mIsTransformDirty{true};

    Vector3 mNormal;
    float mDistanceToOrigin{0.0f};
};