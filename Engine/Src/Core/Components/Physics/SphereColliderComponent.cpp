#include "Engine/Core/Components/Physics/SphereColliderComponent.h"
#include "Engine/Core/Actors/Actor.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Renderer/RenderUtils.h"

SphereColliderComponent::SphereColliderComponent(Actor *actor, PhysicsSystem *physicsSystem, const float radius, const Vector3& offset,
                                                 const bool isStatic, const bool isTrigger)
:ColliderComponent(actor, physicsSystem, Type::Sphere, isStatic, isTrigger)
,mOffset(offset)
,mRadius(radius){}


void SphereColliderComponent::SendDebugDrawCommand(Renderer *renderer, const DrawMode mode) {
    if (!HasFlag(mode, DrawMode::Collision)) {
        return;
    }

    constexpr auto vaType = static_cast<uint32_t>(RenderUtils::DebugType::ColliderPLane);
    const auto key = RenderUtils::GenerateRenderKey(0, vaType, 0, 0);
    const auto transform = GetActor()->GetWorldTransform();

    RenderUtils::DebugCommand cmd = {
        key,
        RenderUtils::DebugType::ColliderSphere,
        transform,
    };

    renderer->PushDebugCommand(cmd);
}

