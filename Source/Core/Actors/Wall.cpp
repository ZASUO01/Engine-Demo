#include "Wall.h"
#include "Engine/Core/Components/Drawing/ModelComponent.h"
#include "Engine/Core/Components/Physics/BoxColliderComponent.h"

Wall::Wall(Model* model, PhysicsSystem *physicsSystem, const Vector3& extents) {
    auto modelComponent = std::make_unique<ModelComponent>(this, model);
    modelComponent->SetTextureIndex(1);

    AddComponent(std::move(modelComponent));
    AddComponent(std::make_unique<BoxColliderComponent>(this, physicsSystem, extents));
}
