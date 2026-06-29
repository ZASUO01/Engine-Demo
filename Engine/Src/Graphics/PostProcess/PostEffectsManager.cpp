#include "Engine/Graphics/PostProcess/PostEffectsManager.h"
#include "Engine/Graphics/PostProcess/PostEffect.h"

PostEffectsManager::PostEffectsManager(): mHasActiveEffect(false){}
PostEffectsManager::~PostEffectsManager() = default;

PostEffect *PostEffectsManager::GetCurrentEffect() const {
    if (mHasActiveEffect) {
        return mPostEffects[mCurrentEffectIdx].get();
    }

    return nullptr;
}

void PostEffectsManager::SetCurrentEffect(const int index) {
    if (index >= 0 && index < mPostEffects.size()) {
        mCurrentEffectIdx = index;
        mHasActiveEffect = true;
    }
}

void PostEffectsManager::AddEffect(std::unique_ptr<PostEffect> effect) {
    if (effect) {
        mPostEffects.emplace_back(std::move(effect));
        mCurrentEffectIdx = static_cast<int>(mPostEffects.size() - 1);
        mHasActiveEffect = true;
    }
}