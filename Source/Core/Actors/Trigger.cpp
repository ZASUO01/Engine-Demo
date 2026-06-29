#include "Trigger.h"
#include "Engine/Core/Components/Physics/BoxColliderComponent.h"

Trigger::Trigger(PhysicsSystem *physicsSystem, const Vector3 &extents, const std::function<void()> &callback)  {
   auto boxCollider = std::make_unique<BoxColliderComponent>(this, physicsSystem, extents, true);
   boxCollider->SetCollisionCallBack(callback);

   AddComponent(std::move(boxCollider));
}
