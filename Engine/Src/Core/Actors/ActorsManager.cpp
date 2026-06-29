#include "Engine/Core/Actors/ActorsManager.h"

#include <algorithm>

#include "Engine/Core/Actors/Actor.h"

ActorsManager::ActorsManager() = default;
ActorsManager::~ActorsManager() {
    UnloadActors();
}

Actor* ActorsManager::AddActor(std::unique_ptr<Actor> actor) {
    if (mIsUpdatingActors) {
        mPendingActors.emplace_back(std::move(actor));
        return mPendingActors.back().get();
    }

    mActors.emplace_back(std::move(actor));
    std::ranges::sort(mActors, [](auto& a, auto& b) {
        return a->GetUpdateOrder() < b->GetUpdateOrder();
    });

    return mActors.back().get();
}

void ActorsManager::UpdateActors(const float deltaTime, Renderer* renderer) {
    // update actors
    mIsUpdatingActors = true;
    for (const auto& actor: mActors) {
        actor->Update(deltaTime, renderer, mDrawMode);
    }

    // remove deleted actors
    for (size_t i = 0; i < mActors.size();) {
        if (mActors[i]->GetState() == Actor::State::Destroy) {
            RemoveActor(static_cast<int>(i));
        }else {
            ++i;
        }
    }

    // move pending actors to the actors list
    if (!mPendingActors.empty()) {
        mActors.reserve(mActors.size() + mPendingActors.size());

        for (auto& actor: mPendingActors) {
            mActors.emplace_back(std::move(actor));
        }
        mPendingActors.clear();
        std::ranges::sort(mActors, [](auto& a, auto& b) {
            return a->GetUpdateOrder() < b->GetUpdateOrder();
        });
    }
    mIsUpdatingActors = false;
}

void ActorsManager::SendInputToActors(const Uint8 *keyState) const {
    for (const auto& actor: mActors) {
        actor->ProcessInput(keyState);
    }
}

void ActorsManager::UnloadActors() {
    mActors.clear();
    mPendingActors.clear();
}


void ActorsManager::RemoveActor(const int index) {
    if (index < 0 || index >= mActors.size()) {
        return;
    }

    if (index < mActors.size() -1) {
        std::swap(mActors[index], mActors.back());
    }

    mActors.pop_back();
}
