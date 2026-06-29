#include "Engine/Core/Components/Physics/ColliderComponent.h"
#include "Engine/Physics/PhysicsSystem.h"

ColliderComponent::ColliderComponent(Actor *actor, PhysicsSystem *physicsSystem, const Type type, const bool isStatic, const bool isTrigger)
:Component(actor)
,mType(type)
,mIsStatic(isStatic)
,mIsTrigger(isTrigger)
,mPhysicsSystem(physicsSystem)
{
    mPhysicsSystem->RegisterCollider(this);
}

ColliderComponent::~ColliderComponent() {
    mPhysicsSystem->UnregisterCollider(this);
}
