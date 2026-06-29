#pragma once
#include <functional>
#include "Engine/Core/Actors/Actor.h"

class PhysicsSystem;
class ActorsManager;

class Trigger : public Actor {
public:
    explicit Trigger(PhysicsSystem* physicsSystem, const Vector3& extents, const std::function<void()> &callback);
};
