#pragma once
#include "Engine/Core/Actors/Actor.h"

class ActorsManager;
class PhysicsSystem;
class Wall;
class Model;

class Desk : public Actor {
public:
    struct DeskModels {
        Model* baseModel{nullptr};
        Model* wallModel{nullptr};
        Model* dividerModel{nullptr};
    };


    explicit Desk(DeskModels& models,  ActorsManager* manager, PhysicsSystem* physicsSystem);

    [[nodiscard]] int GetCurrentOption() const { return currentOption; }
    void ResetOption() { currentOption = -1; }

protected:
    void OnProcessInput(const Uint8 *keyState) override;
    void OnUpdate(float deltaTime) override;

private:
    void SetTriggers(ActorsManager* manager, PhysicsSystem* physicsSystem);
    void SetWalls(Model* model, ActorsManager* manager, PhysicsSystem* physicsSystem);
    void SetDividers(Model* model, ActorsManager* manager, PhysicsSystem* physicsSystem);

    float mInputIntentX{0.0f};
    float mInputIntentZ{0.0f};

    float mSpinSpeed{1.5f};
    float mMaxSpin{Math::PiOver2 / 2};

    int currentOption{-1};
};