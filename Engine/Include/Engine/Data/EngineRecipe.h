#pragma once
#include "LoadingRecipe.h"

class EngineRecipe : public LoadingRecipe {
public:
    ~EngineRecipe() override;

protected:
    void OnLoad(ConfigManager& configManager) override;
    void OnBuild(ResourcesContext &ctx) override;
    void OnCleanup(ResourcesContext &ctx) override {}
};