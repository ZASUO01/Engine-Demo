#pragma once
#include <vector>

#include "Engine/Math/Math.h"

class ColliderComponent;
class SphereColliderComponent;
class PlaneColliderComponent;
class BoxColliderComponent;

class PhysicsSystem {
public:
    PhysicsSystem();
    ~PhysicsSystem();

    void RegisterCollider(ColliderComponent* collider);
    void UnregisterCollider(ColliderComponent* collider);

private:
    void Update() const;
    static void DispatchCollision(ColliderComponent* a, ColliderComponent* b);

    std::vector<ColliderComponent*> mDynamicColliders;
    std::vector<ColliderComponent*> mStaticColliders;

    static void ProcessSpherePlane(const SphereColliderComponent* sphere, const PlaneColliderComponent* plane);
    static bool CheckSpherePlane(const SphereColliderComponent* sphere, const PlaneColliderComponent* plane, float& distance);
    static void ResolveSpherePlane(const SphereColliderComponent* sphere, const PlaneColliderComponent* plane, float distance);

    static void ProcessSphereBox(const SphereColliderComponent* sphere, const BoxColliderComponent* box);
    static bool BroadCheckSphereBox(const SphereColliderComponent* sphere, const BoxColliderComponent* box);
    static bool NarrowCheckSphereBox(const SphereColliderComponent* sphere, const BoxColliderComponent* box, float& distance, Vector3& localDistanceVec);
    static void ResolveSphereBox(const SphereColliderComponent* sphere, const BoxColliderComponent* box, float distance, const Vector3& localDistanceVec);


    friend class Engine;
};