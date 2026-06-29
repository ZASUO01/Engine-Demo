#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Core/Actors/Actor.h"
#include "Engine/Core/Components/Physics/BoxColliderComponent.h"
#include "Engine/Core/Components/Physics/ColliderComponent.h"
#include "Engine/Core/Components/Physics/PlaneColliderComponent.h"
#include "Engine/Core/Components/Physics/RigidBodyComponent.h"
#include "Engine/Core/Components/Physics/SphereColliderComponent.h"

PhysicsSystem::PhysicsSystem() = default;
PhysicsSystem::~PhysicsSystem() = default;

void PhysicsSystem::RegisterCollider(ColliderComponent *collider) {
    if (!collider) {
        return;
    }

    if (collider->IsStatic()) {
        mStaticColliders.push_back(collider);
    }else {
        mDynamicColliders.push_back(collider);
    }
}

void PhysicsSystem::UnregisterCollider(ColliderComponent *collider) {
    if (!collider) {
        return;
    }

    if (collider->IsStatic()) {
        std::erase(mStaticColliders, collider);
    }else {
        std::erase(mDynamicColliders, collider);
    }
}

void PhysicsSystem::Update() const {
    const auto comparisonAPossible = mDynamicColliders.size() >= 2;
    const auto comparisonBPossible = !mDynamicColliders.empty() && !mStaticColliders.empty();

    if (!comparisonAPossible && !comparisonBPossible) {
        return;
    }

    // Compare Dynamic x Dynamic
    if (comparisonAPossible) {
        for (size_t i = 0; i < mDynamicColliders.size(); ++i) {
            ColliderComponent* colliderA = mDynamicColliders[i];

            for (size_t j = i + 1; j < mDynamicColliders.size(); ++j) {
                ColliderComponent* colliderB = mDynamicColliders[j];

                DispatchCollision(colliderA, colliderB);
            }
        }
    }

    // Compare Dynamic x Static
    if (comparisonBPossible) {
        for (ColliderComponent* colliderA : mDynamicColliders) {
            for (ColliderComponent* colliderB : mStaticColliders) {
                DispatchCollision(colliderA, colliderB);
            }
        }
    }
}

void PhysicsSystem::DispatchCollision(ColliderComponent *a, ColliderComponent *b) {
    const auto typeA = a->GetType();
    const auto typeB = b->GetType();

    // Sphere plane
    if (typeA == ColliderComponent::Type::Sphere && typeB == ColliderComponent::Type::Plane) {
        ProcessSpherePlane(dynamic_cast<SphereColliderComponent*>(a), dynamic_cast<PlaneColliderComponent*>(b));
    }else if (typeA == ColliderComponent::Type::Plane && typeB == ColliderComponent::Type::Sphere) {
        ProcessSpherePlane(dynamic_cast<SphereColliderComponent*>(b), dynamic_cast<PlaneColliderComponent*>(a));
    }


    // Sphere box
    if (typeA == ColliderComponent::Type::Sphere && typeB == ColliderComponent::Type::Box) {
        ProcessSphereBox(dynamic_cast<SphereColliderComponent*>(a), dynamic_cast<BoxColliderComponent*>(b));
    }else if (typeA == ColliderComponent::Type::Box && typeB == ColliderComponent::Type::Sphere) {
        ProcessSphereBox(dynamic_cast<SphereColliderComponent*>(b), dynamic_cast<BoxColliderComponent*>(a));
    }
}

void PhysicsSystem::ProcessSpherePlane(const SphereColliderComponent *sphere, const PlaneColliderComponent *plane) {
    if (!sphere || !plane) {
        return;
    }

    if (float distance = 0.0f; CheckSpherePlane(sphere, plane, distance)) {
        ResolveSpherePlane(sphere, plane, distance);
    }
}

bool PhysicsSystem::CheckSpherePlane(const SphereColliderComponent *sphere, const PlaneColliderComponent *plane, float& distance) {
    const auto sphereActor = sphere->GetActor();
    if (!sphereActor) {
        return false;
    }

    const Vector3 position = sphereActor->GetPosition();
    distance = Vector3::Dot(position, plane->GetNormal()) - plane->GetDistanceToOrigin();
    if (distance <= sphere->GetRadius()) {
        return true;
    }

    return false;
}

void PhysicsSystem::ResolveSpherePlane(const SphereColliderComponent *sphere, const PlaneColliderComponent *plane, const float distance) {
    sphere->OnCollisionCallBack();
    plane->OnCollisionCallBack();

    if (sphere->IsTrigger() || plane->IsTrigger()) {
        return;
    }

    const auto rigidBody = sphere->GetRigidBody();
    if (!rigidBody) {
        return;
    }

    const auto sphereActor = sphere->GetActor();
    const Vector3 position = sphereActor->GetPosition();

    constexpr float slop = 0.01f;
    constexpr float percent = 0.8f;

    const float penetrationDepth = Math::Max(sphere->GetRadius() - distance - slop, 0.0f);

    sphereActor->SetPosition(position + plane->GetNormal() * (penetrationDepth* percent));

    const Vector3 velocity = rigidBody->GetVelocity();
    const float velocityAlongNormal = Vector3::Dot(velocity, plane->GetNormal());

    if (velocityAlongNormal > 0.0f) {
        return;
    }

    const float restitution = rigidBody->GetRestitutionCoefficient();

    const Vector3 impulse = plane->GetNormal() * (-(1.0f + restitution) * velocityAlongNormal);
    rigidBody->SetVelocity(velocity + impulse);
}

void PhysicsSystem::ProcessSphereBox(const SphereColliderComponent *sphere, const BoxColliderComponent *box) {
    if (!sphere || !box) {
        return;
    }

    if (!sphere->GetActor() || !box->GetActor()) {
        return;
    }

    if (!BroadCheckSphereBox(sphere, box)) {
        return;
    }

    Vector3 localDistanceVec;

    if (float distance = 0.0f; NarrowCheckSphereBox(sphere, box, distance, localDistanceVec)) {
        ResolveSphereBox(sphere, box, distance, localDistanceVec);
    }
}

bool PhysicsSystem::BroadCheckSphereBox(const SphereColliderComponent *sphere, const BoxColliderComponent *box) {
    const auto sphereActor = sphere->GetActor();
    const auto boxActor = box->GetActor();

    const Matrix4& sphereWorldTransform = sphereActor->GetWorldTransform();
    const Vector3 worldSpherePos = sphereWorldTransform.GetTranslation();
    const float sphereRadius = sphere->GetRadius();

    const Matrix4& boxWorldTransform = boxActor->GetWorldTransform();
    const Vector3 worldBoxPos = boxWorldTransform.GetTranslation();
    const Vector3 extents = box->GetExtents();

    const float boxBoundingRadius = Math::Sqrt(extents.x * extents.x +
                                               extents.y * extents.y +
                                               extents.z * extents.z);


    const float maxCollisionDist = sphereRadius + boxBoundingRadius;
    const float maxCollisionDistSq = maxCollisionDist * maxCollisionDist;

    const Vector3 centerToCenter = worldSpherePos - worldBoxPos;

    if (const float actualDistSq = Vector3::Dot(centerToCenter, centerToCenter); actualDistSq <= maxCollisionDistSq) {
        return true;
    }

    return false;
}

bool PhysicsSystem::NarrowCheckSphereBox(const SphereColliderComponent *sphere, const BoxColliderComponent *box, float& distance, Vector3& localDistanceVec) {
    const auto sphereActor = sphere->GetActor();
    const auto boxActor = box->GetActor();

    const Matrix4& sphereWorldTransform = sphereActor->GetWorldTransform();
    const Vector3 worldSpherePos = sphereWorldTransform.GetTranslation();
    const Matrix4& boxWorldTransform = boxActor->GetWorldTransform();
    const float sphereRadius = sphere->GetRadius();

    Matrix4 inverseBoxTransform = boxWorldTransform;
    inverseBoxTransform.Invert();

    const Vector3 localSpherePos = inverseBoxTransform.TransformPoint(worldSpherePos);

    const Vector3 extents = box->GetExtents();
    Vector3 closestLocalPoint;
    closestLocalPoint.x = Math::Clamp(localSpherePos.x, -extents.x, extents.x);
    closestLocalPoint.y = Math::Clamp(localSpherePos.y, -extents.y, extents.y);
    closestLocalPoint.z = Math::Clamp(localSpherePos.z, -extents.z, extents.z);

    localDistanceVec = localSpherePos - closestLocalPoint;
    distance = Vector3::Dot(localDistanceVec, localDistanceVec);

    if (distance < sphereRadius * sphereRadius) {
        return true;
    }

    return false;
}


void PhysicsSystem::ResolveSphereBox(const SphereColliderComponent *sphere, const BoxColliderComponent *box, float distance, const Vector3& localDistanceVec) {
        sphere->OnCollisionCallBack();
        box->OnCollisionCallBack();

        if (sphere->IsTrigger() || box->IsTrigger()) {
            return;
        }

        const auto rigidBody = sphere->GetRigidBody();
        if (!rigidBody) {
            return;
        }

        distance = Math::Sqrt(distance);

        if (distance <= 0.0001f) {
            distance = 0.0001f;
        }

        const float invDistance = 1.0f / distance;
        const auto localNormal = Vector3(
            localDistanceVec.x * invDistance,
            localDistanceVec.y * invDistance,
            localDistanceVec.z * invDistance
        );

        const auto sphereActor = sphere->GetActor();
        const auto boxActor = box->GetActor();
        const float sphereRadius = sphere->GetRadius();
        const Matrix4& boxWorldTransform = boxActor->GetWorldTransform();

        Vector3 globalNormal = boxWorldTransform.TransformVector(localNormal);
        globalNormal.Normalize();

        const float penetration = sphereRadius - distance;

        const Vector3 currentPos = sphereActor->GetPosition();
        sphereActor->SetPosition(currentPos + globalNormal * penetration);

        const Vector3 velocity = rigidBody->GetVelocity();
        const float velocityAlongNormal = Vector3::Dot(velocity, globalNormal);

        if (velocityAlongNormal > 0.0f) {
            return;
        }

        const float sphereRestitution = rigidBody->GetRestitutionCoefficient();
        const float boxRestitution = box->GetRestitution();
        const float combinedRestitution = Math::Max(sphereRestitution, boxRestitution);
        const float minRestitution = box->GetMinRestitution();

        float j = -(1.0f + combinedRestitution) * velocityAlongNormal;

        if (boxRestitution >= 1.0f && Math::Abs(velocityAlongNormal) < minRestitution) {
            const float energyInjection = minRestitution - Math::Abs(velocityAlongNormal);
            j += energyInjection;
        }

        const Vector3 impulse = globalNormal * j;
        rigidBody->SetVelocity(velocity + impulse);
}