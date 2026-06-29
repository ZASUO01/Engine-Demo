#include "Ball.h"
#include "Engine/Core/Components/Drawing/ModelComponent.h"
#include "Engine/Core/Components/Physics/RigidBodyComponent.h"
#include "Engine/Core/Components/Physics/SphereColliderComponent.h"
#include "Engine/Physics/PhysicsSystem.h"

Ball::Ball(Model* model, PhysicsSystem* physicsSystem) {
    auto modelComponent = std::make_unique<ModelComponent>(this, model);
    modelComponent->SetTextureIndex(1);

    auto rigidBody = std::make_unique<RigidBodyComponent>(this);

    auto collider = std::make_unique<SphereColliderComponent>(this, physicsSystem, 0.1f);
    collider->SetRigidBody(rigidBody.get());

    AddComponent(std::move(modelComponent));
    AddComponent(std::move(collider));
    AddComponent(std::move(rigidBody));

    SetScale(Vector3{0.1f, 0.1f, 0.1f});
}
