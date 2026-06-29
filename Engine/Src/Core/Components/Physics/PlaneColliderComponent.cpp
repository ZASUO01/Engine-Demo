#include "Engine/Core/Components/Physics/PlaneColliderComponent.h"

#include <SDL_log.h>

#include "Engine/Core/Actors/Actor.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Renderer/RenderUtils.h"

PlaneColliderComponent::PlaneColliderComponent(Actor *actor, PhysicsSystem *physicsSystem, const bool isTrigger)
:ColliderComponent(actor, physicsSystem, Type::Plane, true, isTrigger)
{
    CalculateFromOwner();
}

void PlaneColliderComponent::Update(float deltaTime) {
    CalculateFromOwner();
}

void PlaneColliderComponent::SendDebugDrawCommand(Renderer *renderer, const DrawMode mode) {
    if (!HasFlag(mode, DrawMode::Collision)) {
        return;
    }

    constexpr auto vaType = static_cast<uint32_t>(RenderUtils::DebugType::ColliderPLane);
    const auto key = RenderUtils::GenerateRenderKey(0, vaType, 0, 0);
    const auto transform = GetActor()->GetWorldTransform();

    RenderUtils::DebugCommand cmd = {
        key,
        RenderUtils::DebugType::ColliderPLane,
        transform,
    };

    renderer->PushDebugCommand(cmd);
}

void PlaneColliderComponent::CalculateFromOwner() {
    const auto owner = GetActor();
    if (!owner) {
        return;
    }

    const Vector3 rotation = owner->GetRotation();
    const Vector3 position = owner->GetPosition();

    const float cx = Math::Cos(rotation.x);
    const float sx = Math::Sin(rotation.x);
    const float cy = Math::Cos(rotation.y);
    const float sy = Math::Sin(rotation.y);
    const float cz = Math::Cos(rotation.z);
    const float sz = Math::Sin(rotation.z);

    mNormal.x = (-sz * cy) + (sx * cz * sy);
    mNormal.y = (cx * cz);
    mNormal.z = (sz * sy) + (sx * cz * cy);

    mNormal.Normalize();
    mDistanceToOrigin = Vector3::Dot(mNormal, position);
}