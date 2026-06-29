#pragma once
#include "Engine/Core/Actors/Actor.h"

class PhysicsSystem;
class ActorsManager;
class Model;

class Wall : public Actor {
public:
    explicit Wall(Model* model, PhysicsSystem* physicsSystem, const Vector3& extents);
};
