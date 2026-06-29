#include "Desk.h"
#include "Engine/Core/Components/Drawing/ModelComponent.h"
#include "Engine/Core/Components/Physics/PlaneColliderComponent.h"
#include <SDL2/SDL_keyboard.h>
#include "Divider.h"
#include "Trigger.h"
#include "Wall.h"
#include "Engine/Core/Actors/ActorsManager.h"
#include "Engine/Core/Components/Physics/BoxColliderComponent.h"


Desk::Desk(DeskModels& models, ActorsManager* manager, PhysicsSystem *physicsSystem) {
    auto modelComponent = std::make_unique<ModelComponent>(this, models.baseModel);
    modelComponent->SetTextureIndex(1);

    AddComponent(std::move(modelComponent));
    AddComponent(std::make_unique<PlaneColliderComponent>(this, physicsSystem));

    SetTriggers(manager, physicsSystem);
    SetWalls(models.wallModel, manager, physicsSystem);
    SetDividers(models.dividerModel, manager, physicsSystem);
}

void Desk::OnProcessInput(const Uint8 *keyState) {
    mInputIntentX = 0.0f;
    mInputIntentZ = 0.0f;

    if (keyState[SDL_SCANCODE_W]) mInputIntentX -= 1.0f;
    if (keyState[SDL_SCANCODE_S]) mInputIntentX += 1.0f;

    if (keyState[SDL_SCANCODE_A]) mInputIntentZ -= 1.0f;
    if (keyState[SDL_SCANCODE_D]) mInputIntentZ += 1.0f;
}

void Desk::OnUpdate(const float deltaTime) {
    Vector3 currentRotation = GetRotation();

    if (mInputIntentX != 0.0f) {
        currentRotation.x += mInputIntentX * mSpinSpeed * deltaTime;
    }else {
        currentRotation.x -= Math::Clamp(currentRotation.x, -mSpinSpeed * deltaTime, mSpinSpeed * deltaTime);
    }

    if (mInputIntentZ != 0.0f) {
        currentRotation.z += mInputIntentZ * mSpinSpeed * deltaTime;
    } else {
        currentRotation.z -= Math::Clamp(currentRotation.z, -mSpinSpeed * deltaTime, mSpinSpeed * deltaTime);
    }

    currentRotation.x = Math::Clamp(currentRotation.x, -mMaxSpin, mMaxSpin);
    currentRotation.z = Math::Clamp(currentRotation.z, -mMaxSpin, mMaxSpin);
    currentRotation.y = 0.0f;

    SetRotation(currentRotation);
}

void Desk::SetTriggers(ActorsManager *manager, PhysicsSystem *physicsSystem) {
    // Triggers
    auto northTrigger = std::make_unique<Trigger>(physicsSystem, Vector3{0.85f, 0.5f, 0.5f}, [this]() {
        currentOption = 0;
    });
    northTrigger->SetPosition(Vector3{0.0f, 0.0f, -1.4f});
    northTrigger->SetParent(this);
    manager->AddActor(std::move(northTrigger));

    auto southTrigger = std::make_unique<Trigger>(physicsSystem, Vector3{0.85f, 0.5f, 0.5f}, [this]() {
        currentOption = 1;
    });
    southTrigger->SetPosition(Vector3{0.0f, 0.0f, 1.4f});
    southTrigger->SetParent(this);
    manager->AddActor(std::move(southTrigger));

    auto eastTrigger = std::make_unique<Trigger>(physicsSystem, Vector3{0.5f, 0.5f, 0.85f}, [this] {
        currentOption = 2;
    });
    eastTrigger->SetPosition(Vector3{1.4f, 0.0f, 0.0f});
    eastTrigger->SetParent(this);
    manager->AddActor(std::move(eastTrigger));

    auto westTrigger = std::make_unique<Trigger>(physicsSystem, Vector3{0.5f, 0.5f, 0.85f}, [this] {
        currentOption = 3;
    });
    westTrigger->SetPosition(Vector3{-1.4f, 0.0f, 0.0f});
    westTrigger->SetParent(this);
    manager->AddActor(std::move(westTrigger));
}

void Desk::SetWalls(Model *model, ActorsManager *manager, PhysicsSystem *physicsSystem) {
    auto southWall = std::make_unique<Wall>(model, physicsSystem, Vector3{2.0f, 0.5f, 0.1f});
    southWall->SetPosition(Vector3{0.0f, 0.0f, 1.95f});
    southWall->SetParent(this);
    manager->AddActor(std::move(southWall));

    auto northWall = std::make_unique<Wall>(model, physicsSystem, Vector3{2.0f, 0.5f, 0.1f});
    northWall->SetPosition(Vector3{0.0f, 0.0f, -1.95f});
    northWall->SetParent(this);
    manager->AddActor(std::move(northWall));

    auto westWall = std::make_unique<Wall>(model, physicsSystem, Vector3{2.0f, 0.5f, 0.1f});
    westWall->SetPosition(Vector3{1.95f, 0.0f, 0.0f});
    westWall->SetRotation(Vector3{0.0f, Math::PiOver2, 0.0f});
    westWall->SetParent(this);
    manager->AddActor(std::move(westWall));

    auto eastWall = std::make_unique<Wall>(model, physicsSystem, Vector3{2.0f, 0.5f, 0.1f});
    eastWall->SetPosition(Vector3{-1.95f, 0.0f, 0.0f});
    eastWall->SetRotation(Vector3{0.0f, Math::PiOver2, 0.0f});
    eastWall->SetParent(this);
    manager->AddActor(std::move(eastWall));
}

void Desk::SetDividers(Model *model, ActorsManager *manager, PhysicsSystem *physicsSystem) {
    auto northEastDivider = std::make_unique<Divider>(model, physicsSystem, Vector3{0.5f, 0.5, 0.5f});
    northEastDivider->SetPosition(Vector3{1.4f, 0.0f, -1.4f});
    northEastDivider->SetParent(this);
    manager->AddActor(std::move(northEastDivider));

    auto northWestDivider = std::make_unique<Divider>(model, physicsSystem, Vector3{0.5f, 0.5, 0.5f});
    northWestDivider->SetPosition(Vector3{-1.4f, 0.0f, -1.4f});
    northWestDivider->SetParent(this);
    manager->AddActor(std::move(northWestDivider));

    auto southEastDivider = std::make_unique<Divider>(model, physicsSystem, Vector3{0.5f, 0.5, 0.5f});
    southEastDivider->SetPosition(Vector3{1.4f, 0.0f, 1.4f});
    southEastDivider->SetParent(this);
    manager->AddActor(std::move(southEastDivider));

    auto southWestDivider = std::make_unique<Divider>(model, physicsSystem, Vector3{0.5f, 0.5, 0.5f});
    southWestDivider->SetPosition(Vector3{-1.4f, 0.0f, 1.4f});
    southWestDivider->SetParent(this);
    manager->AddActor(std::move(southWestDivider));
}
