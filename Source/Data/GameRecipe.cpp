#include "GameRecipe.h"
#include "Engine/Assets/AssetsManager.h"
#include "Engine/Config/ConfigManager.h"
#include "Engine/Contexts/Contexts.h"
#include "Engine/Graphics/Model.h"

GameRecipe::~GameRecipe() = default;

void GameRecipe::OnLoad(ConfigManager& configManager) {
    if (const auto filterShaderName = configManager.GetString("game.shaders.filter"); !filterShaderName.empty()) {
        LoadShaderStrings(filterShaderName);
    }

    if (const auto meshName = configManager.GetString("game.assets.models.sphere"); !meshName.empty()) {
        LoadObjectVertices(meshName);
    }
    if (const auto texName = configManager.GetString("game.assets.textures.sphere"); !texName.empty()) {
        LoadTextureSurface(texName);
    }

    if (const auto meshName = configManager.GetString("game.assets.models.base"); !meshName.empty()) {
        LoadObjectVertices(meshName);
    }
    if (const auto texName = configManager.GetString("game.assets.textures.base"); !texName.empty()) {
        LoadTextureSurface(texName);
    }

    if (const auto meshName = configManager.GetString("game.assets.models.wall"); !meshName.empty()) {
        LoadObjectVertices(meshName);
    }

    if (const auto meshName = configManager.GetString("game.assets.models.divider"); !meshName.empty()) {
        LoadObjectVertices(meshName);
    }
    if (const auto texName = configManager.GetString("game.assets.textures.alter"); !texName.empty()) {
        LoadTextureSurface(texName);
    }
}

void GameRecipe::OnBuild(ResourcesContext &ctx) {

}
