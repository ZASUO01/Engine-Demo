#include "Engine/Core/Components/Physics/BoxColliderComponent.h"

#include "Engine/Core/Actors/Actor.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Renderer/RenderUtils.h"

BoxColliderComponent::BoxColliderComponent(Actor *actor, PhysicsSystem *physicsSystem, const Vector3 &extents, const bool isTrigger)
:ColliderComponent(actor, physicsSystem, Type::Box, true, isTrigger)
,mExtents(extents)
{}

void BoxColliderComponent::SendDebugDrawCommand(Renderer *renderer, const DrawMode mode) {
    if (!HasFlag(mode, DrawMode::Collision)) {
        return;
    }

    constexpr auto valType = static_cast<uint32_t>(RenderUtils::DebugType::ColliderBox);
    const auto key = RenderUtils::GenerateRenderKey(0, valType, 0, 0);

    const auto scale = Matrix4::CreateScale(mExtents);
    const auto transform = scale * GetActor()->GetWorldTransform();

    RenderUtils::DebugCommand cmd = {
        key,
        RenderUtils::DebugType::ColliderBox,
        transform
    };

    if (IsTrigger()) {
        cmd.isTrigger = true;
    }

    renderer->PushDebugCommand(cmd);
}