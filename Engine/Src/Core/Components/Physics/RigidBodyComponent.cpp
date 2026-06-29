#include "Engine/Core/Components/Physics/RigidBodyComponent.h"

#include "Engine/Core/Actors/Actor.h"

RigidBodyComponent::RigidBodyComponent(Actor *actor)
:Component(actor)
,mApplyGravity(true)
{}

RigidBodyComponent::~RigidBodyComponent() = default;

void RigidBodyComponent::ApplyForce(const Vector3 &force) {
    mAcceleration += force * (1.0f / mMass);
}

void RigidBodyComponent::Update(const float deltaTime) {
    if (mApplyGravity) {
        const Vector3 gravityForce = Vector3::UnitY * (-mGravity * mMass);
        ApplyForce(gravityForce);
    }

    mVelocity.x *= Math::Clamp(1.0f - mFrictionCoefficient.x * deltaTime, 0.0f, 1.0f);
    mVelocity.y *= Math::Clamp(1.0f - mFrictionCoefficient.y * deltaTime, 0.0f, 1.0f);
    mVelocity.z *= Math::Clamp(1.0f - mFrictionCoefficient.z * deltaTime, 0.0f, 1.0f);

    mVelocity += mAcceleration * deltaTime;

    mVelocity.x =  Math::Clamp(mVelocity.x, -mMaxVelocity.x, mMaxVelocity.x);
    mVelocity.y = Math::Clamp(mVelocity.y, -mMaxVelocity.y, mMaxVelocity.y);
    mVelocity.z =Math::Clamp(mVelocity.z, -mMaxVelocity.z, mMaxVelocity.z);

    if (Math::NearlyZero(mVelocity.x)) {
        mVelocity.x = 0.0f;
    }

    if (Math::NearlyZero(mVelocity.y)) {
        mVelocity.y = 0.0f;
    }

    if (Math::NearlyZero(mVelocity.z)) {
        mVelocity.z = 0.0f;
    }

    const auto actor = GetActor();

    actor->SetPosition(actor->GetPosition() + mVelocity * deltaTime);
    mAcceleration = Vector3::Zero;
}
