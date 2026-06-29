#include "Engine/Data/EngineRecipe.h"
#include "Engine/Assets/AssetsManager.h"
#include "Engine/Config/ConfigManager.h"

EngineRecipe::~EngineRecipe() = default;

void EngineRecipe::OnLoad(ConfigManager& configManager) {
    if (const auto str = configManager.GetString("engine.shaders.shadow"); !str.empty()) {
        LoadShaderStrings(str);
    }

    if (const auto str = configManager.GetString("engine.shaders.models"); !str.empty()) {
        LoadShaderStrings(str);
    }

    if (const auto str = configManager.GetString("engine.shaders.shadowModels"); !str.empty()) {
        LoadShaderStrings(str);
    }

    if (const auto str = configManager.GetString("engine.shaders.ui"); !str.empty()) {
        LoadShaderStrings(str);
    }

    if (const auto str = configManager.GetString("engine.shaders.debug"); !str.empty()) {
        LoadShaderStrings(str);
    }

    if (const auto str = configManager.GetString("engine.assets.models.default"); !str.empty()) {
        LoadObjectVertices(str);
    }

    if (const auto str = configManager.GetString("engine.assets.textures.default"); !str.empty()) {
        LoadTextureSurface(str);
    }

    if (const auto str = configManager.GetString("engine.assets.fonts.mono"); !str.empty()) {
        LoadTTFFont(str);
    }
}

void EngineRecipe::OnBuild(ResourcesContext &ctx) {

}
