#pragma once
#include <functional>

#include "Engine/Core/Components/Component.h"

class PhysicsSystem;
class Shader;
class VertexArray;

class ColliderComponent : public Component {
public:
    enum class Type {
        Sphere,
        Plane,
        Box,
    };

    explicit ColliderComponent(Actor* actor, PhysicsSystem* physicsSystem, Type type, bool isStatic = false, bool isTrigger = false);
    ~ColliderComponent() override;

    [[nodiscard]] bool IsStatic() const { return mIsStatic;}
    [[nodiscard]] bool IsTrigger() const { return mIsTrigger; }
    [[nodiscard]] Type GetType() const { return mType; }

    void OnCollisionCallBack() const { if (mCollisionCallBack) { mCollisionCallBack(); } }
    void SetCollisionCallBack(const std::function<void()> &callback) { mCollisionCallBack = callback; }

    void SetRestitution(const float restitution) { mRestitution = restitution; }
    [[nodiscard]] float GetRestitution() const { return mRestitution; }

    [[nodiscard]] float GetMinRestitution() const { return mMinRestitutionVelocity; }
private:
    Type mType;
    bool mIsStatic;
    bool mIsTrigger;

    float mRestitution{0.0f};
    float mMinRestitutionVelocity{15.0f};

    PhysicsSystem* mPhysicsSystem;
    std::function<void()> mCollisionCallBack{nullptr};
};
