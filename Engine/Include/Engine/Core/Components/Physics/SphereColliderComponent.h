#pragma once
#include "ColliderComponent.h"

class RigidBodyComponent;

class SphereColliderComponent : public ColliderComponent {
public:
    explicit SphereColliderComponent(Actor* actor, PhysicsSystem* physicsSystem,
        float radius, const Vector3& offset = Vector3::Zero, bool isStatic = false, bool isTrigger = false);

    [[nodiscard]] float GetRadius() const { return mRadius; }
    [[nodiscard]] const Vector3& GetOffset() const { return mOffset; }

    void SetRigidBody(RigidBodyComponent* rigidBodyComponent) { if ( rigidBodyComponent) { mRigidBody = rigidBodyComponent; } }
    [[nodiscard]] RigidBodyComponent* GetRigidBody() const { return mRigidBody; }

    void SendDebugDrawCommand(Renderer *renderer, DrawMode mode) override;
private:
    Vector3 mOffset;
    float mRadius;
    RigidBodyComponent* mRigidBody{nullptr};
};
