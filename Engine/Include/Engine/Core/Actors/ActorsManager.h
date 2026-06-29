#pragma once
#include <memory>
#include <vector>

#include "Engine/Config/ConfigFlags.h"
#include "Engine/Fwd/SDLFwd.h"

class Actor;

class ActorsManager {
public:
    ActorsManager();
    ~ActorsManager();

    Actor* AddActor(std::unique_ptr<Actor> actor);
    void UnloadActors();

private:
    void UpdateActors(float deltaTime, class Renderer* renderer);
    void SendInputToActors(const Uint8 *keyState) const;
    void SetDrawMode(const DrawMode mode) { mDrawMode = mode; }

    void RemoveActor(int index);

    std::vector<std::unique_ptr<Actor>> mActors;
    std::vector<std::unique_ptr<Actor>> mPendingActors;
    bool mIsUpdatingActors{false};

    DrawMode mDrawMode{DrawMode::None};
    friend class Engine;
};
