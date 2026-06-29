#pragma once
#include "Engine/Core/Components/Component.h"

class RigidBodyComponent : public Component {
public:
    explicit RigidBodyComponent(Actor *actor);
    ~RigidBodyComponent() override;

    void Update(float deltaTime) override;

    [[nodiscard]] const Vector3& GetVelocity() const { return mVelocity; }
    void SetVelocity(const Vector3& velocity) { mVelocity = velocity; }
    void SetMaxVelocity(const Vector3 &maxVelocity) { mMaxVelocity = maxVelocity; }

    [[nodiscard]] const Vector3& GetAcceleration() const { return mAcceleration; }
    void SetAcceleration(const Vector3& acceleration) { mAcceleration = acceleration; }

    void SetApplyGravity(const bool applyGravity) { mApplyGravity = applyGravity; }
    void SetMass(const float mass) { mMass = mass; }
    void SetGravity(const float gravity) { mGravity = gravity; }
    void SetFriction(const Vector3 &friction) { mFrictionCoefficient = friction; }

    void ApplyForce(const Vector3 &force);

    [[nodiscard]] float GetRestitutionCoefficient() const { return mRestitutionCoefficient; }
    void SetRestitutionCoefficient(const float coefficient) { mRestitutionCoefficient = coefficient; }
private:
    bool mApplyGravity;
    float mGravity{10.0f};
    float mMass{1.0f};
    Vector3 mFrictionCoefficient{Vector3::Zero};
    float mRestitutionCoefficient{0.0f};

    Vector3 mVelocity{Vector3::Zero};
    Vector3 mMaxVelocity{Vector3{1000.0f, 1000.0f, 1000.0f}};
    Vector3 mAcceleration{Vector3::Zero};
};