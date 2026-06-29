#pragma once
#include <memory>
#include <vector>

class PostEffect;

class PostEffectsManager {
public:
    PostEffectsManager();
    ~PostEffectsManager();

    void SetCurrentEffect(int index);
    [[nodiscard]] PostEffect* GetCurrentEffect() const;
    void AddEffect(std::unique_ptr<PostEffect> effect);

    void DisableEffect(){ mCurrentEffectIdx = 0; mHasActiveEffect = false; }
    [[nodiscard]] bool HasActiveEffect() const { return mHasActiveEffect; }
private:
    std::vector<std::unique_ptr<PostEffect>> mPostEffects;
    int mCurrentEffectIdx{0};
    bool mHasActiveEffect;
};
