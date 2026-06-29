#pragma once
#include "Engine/Data/LoadingRecipe.h"

class GameRecipe : public LoadingRecipe {
public:
    ~GameRecipe() override;

protected:
    void OnLoad(ConfigManager& configManager) override;
    void OnBuild(ResourcesContext &ctx) override;
    void OnCleanup(ResourcesContext &ctx) override{}
};

