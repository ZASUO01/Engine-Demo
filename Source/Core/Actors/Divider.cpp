#include "Divider.h"
#include "Engine/Core/Components/Drawing/ModelComponent.h"
#include "Engine/Core/Components/Physics/BoxColliderComponent.h"

Divider::Divider(Model *model, PhysicsSystem *physicsSystem, const Vector3 &extents) {
    auto modelComponent = std::make_unique<ModelComponent>(this, model);
    modelComponent->SetTextureIndex(1);

    auto colliderComponent = std::make_unique<BoxColliderComponent>(this, physicsSystem, extents);
    colliderComponent->SetRestitution(0.8f);

    AddComponent(std::move(modelComponent));
    AddComponent(std::move(colliderComponent));
}
