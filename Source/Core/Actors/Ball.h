#pragma once
#include "Engine/Core/Actors/Actor.h"

class PhysicsSystem;
class Model;

class Ball : public Actor {
public:
    explicit Ball(Model* model, PhysicsSystem* physicsSystem);
};