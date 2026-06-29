#pragma once
#include <atomic>
#include <memory>
#include <queue>
#include <thread>
#include "Engine/Contexts/Contexts.h"

class LoadingRecipe;

class ResourcesLoader {
public:
    enum class State {
        Empty,
        Loading,
        Loaded,
        Ready,
    };

    explicit ResourcesLoader(const ResourcesContext &ctx);
    ~ResourcesLoader();

    void SubmitLoadingInstruction(std::unique_ptr<LoadingRecipe> recipe);

private:
    [[nodiscard]] bool ConcludeLoadingProcess();
    void UnloadResources();
    [[nodiscard]] State GetState() const;

    void ProcessNextInQueue();

    std::unique_ptr<LoadingRecipe> mCurrentRecipe{nullptr};
    std::unique_ptr<LoadingRecipe> mNewRecipe{nullptr};
    std::queue<std::unique_ptr<LoadingRecipe>> mRecipeQueue;

    std::atomic<State> mState{State::Empty};
    std::jthread mLoadingThread;

    ResourcesContext mContext;

    friend class Engine;
};
