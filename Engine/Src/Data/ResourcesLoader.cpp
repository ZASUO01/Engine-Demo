#include "Engine/Data/ResourcesLoader.h"
#include "Engine/Data/LoadingRecipe.h"

ResourcesLoader::ResourcesLoader(const ResourcesContext &ctx)
: mContext(ctx)
{}

ResourcesLoader::~ResourcesLoader() {
    UnloadResources();
}

void ResourcesLoader::SubmitLoadingInstruction(std::unique_ptr<LoadingRecipe> recipe) {
    mRecipeQueue.push(std::move(recipe));

    if (const State currentState = mState.load(std::memory_order_acquire); currentState == State::Empty || currentState == State::Ready) {
        ProcessNextInQueue();
    }
}

void ResourcesLoader::ProcessNextInQueue() {
    if (mRecipeQueue.empty()) {
        return;
    }

    if (mLoadingThread.joinable()) {
        mLoadingThread.request_stop();
        mLoadingThread.join();
    }

    mNewRecipe = std::move(mRecipeQueue.front());
    mRecipeQueue.pop();

    mState.store(State::Loading, std::memory_order_release);

    mLoadingThread = std::jthread([this](const std::stop_token &st) {
       mNewRecipe->LoadData(st, mContext.configContext);

        if (st.stop_requested()) {
            return;
        }

        mState.store(State::Loaded, std::memory_order_release);
    });
}

bool ResourcesLoader::ConcludeLoadingProcess() {
    if (mState.load(std::memory_order_acquire) != State::Loaded) {
        return false;
    }

    if (mCurrentRecipe) {
        mCurrentRecipe->CleanupStructures(mContext);
        mCurrentRecipe.reset();
    }

    mNewRecipe->BuildStructures(mContext);
    mCurrentRecipe = std::move(mNewRecipe);

    if (!mRecipeQueue.empty()) {
        ProcessNextInQueue();
        return false;
    }

    mState.store(State::Ready, std::memory_order_release);
    return true;
}

void ResourcesLoader::UnloadResources() {
    if (mState.load(std::memory_order_acquire) == State::Empty) {
        return;
    }

    if (mLoadingThread.joinable()) {
        mLoadingThread.request_stop();
        mLoadingThread.join();
    }

    if (mCurrentRecipe) {
        mCurrentRecipe->CleanupStructures(mContext);
        mCurrentRecipe.reset();
    }

    if (mNewRecipe) {
        mNewRecipe->CleanupStructures(mContext);
        mNewRecipe.reset();
    }

    mState.store(State::Empty, std::memory_order_release);
}

ResourcesLoader::State ResourcesLoader::GetState() const {
    return mState.load(std::memory_order_acquire);
}
