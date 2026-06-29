#pragma once
#include "Engine/Core/Actors/Actor.h"

class PhysicsSystem;
class ActorsManager;
class Model;

class Divider : public Actor {
public:
    explicit Divider(Model* model, PhysicsSystem* physicsSystem, const Vector3& extents);
};
