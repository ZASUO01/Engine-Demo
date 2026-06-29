#include "Engine/Data/LoadingRecipe.h"
#include "Engine/Assets/AssetsManager.h"
#include "Engine/Graphics/Shader/ShadersManager.h"
#include "Engine/Utils/FileUtils.h"
#include "Engine/Contexts/Contexts.h"
#include "Engine/Graphics/Geometry/Mesh.h"
#include "Engine/Graphics/Resources/Font.h"
#include "Engine/Graphics/Resources/Texture.h"
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Utils/ObjectUtils.h"
#include "Engine/Utils/SDLUtils.h"

LoadingRecipe::~LoadingRecipe() = default;

void LoadingRecipe::LoadShaderStrings(const std::string &name) {
    if (StopRequested() || mPhase.load(std::memory_order_acquire) != Phase::Load) {
        return;
    }

    std::string vertexString;
    if (StopRequested() || !FileUtils::LoadFileToString(name + ".vert", vertexString)) {
        return;
    }

    std::string fragmentString;
    if (StopRequested() || !FileUtils::LoadFileToString(name + ".frag", fragmentString)) {
        return;
    }

    mShaderStrings.emplace_back(std::move(vertexString), std::move(fragmentString), name);
}

void LoadingRecipe::LoadObjectVertices(const std::string &name) {
    if (StopRequested() || mPhase.load(std::memory_order_acquire) != Phase::Load) {
        return;
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    if (StopRequested() || !ObjectUtils::LoadFromFile(name + ".obj", vertices, indices)) {
        return;
    }

    mObjectVertices.emplace_back(std::move(vertices), std::move(indices), name);
}

void LoadingRecipe::LoadTextureSurface(const std::string &name) {
    if (StopRequested() || mPhase.load(std::memory_order_acquire) != Phase::Load) {
        return;
    }

    auto surface = SDLUtils::LoadSurfaceFromImage(name+".png");
    if (StopRequested() || !surface) {
        return;
    }

    mTextureSurfaces.emplace_back(std::move(surface), name);
}

void LoadingRecipe::LoadTTFFont(const std::string &name) {
    static std::vector typicalFontSizes = { 12, 16, 24, 36, 40, 48, 64, 72 };

    std::unordered_map<int, UniqueTTFFont> fontMap;

    for (auto size : typicalFontSizes) {
        auto font = SDLUtils::LoadTTFFontFromFile(name + ".ttf", size);
        if (StopRequested() || !font) {
            return;
        }

       fontMap.emplace(size, std::move(font));
    }

    mFontMaps.emplace_back(std::move(fontMap), name);
}

void LoadingRecipe::LoadData(const std::stop_token &st, ConfigManager& configManager) {
    if (auto expected = Phase::Initial; !mPhase.compare_exchange_strong(expected, Phase::Load, std::memory_order_acq_rel)) {
        return;
    }

    mStopToken = st;
    OnLoad(configManager);

    if (!StopRequested()) {
        mPhase.store(Phase::Build, std::memory_order_release);
    }
}

void LoadingRecipe::BuildStructures(ResourcesContext &ctx) {
    if (mPhase.load(std::memory_order_acquire) != Phase::Build) {
        return;
    }

    auto& shadersContext = ctx.shadersContext;
    for (const auto&[vertexString, fragmentString, name] : mShaderStrings) {
        if (const auto shader = shadersContext.AddShader(name); !shader->Build(vertexString, fragmentString)) {
            shadersContext.RemoveShader(name);
        }
    }

    auto& assetsContext = ctx.assetsContext;
    for (const auto&[vertices, indices, name]: mObjectVertices) {
        if (const auto mesh = assetsContext.AddMesh(name); !mesh->Build(vertices, indices)) {
            assetsContext.RemoveMesh(name);
        }
    }

    for (auto&[surface, name]: mTextureSurfaces) {
        if (const auto texture = assetsContext.AddTexture(name); !texture->BuildFromSurface(surface)) {
            assetsContext.RemoveTexture(name);
        }
    }

    for (auto&[fontMap, name]: mFontMaps) {
        if (const auto font = assetsContext.AddFont(name); !font->Build(std::move(fontMap))) {
            assetsContext.RemoveFont(name);
        }
    }

    OnBuild(ctx);

    mShaderStrings.clear();
    mShaderStrings.shrink_to_fit();

    mObjectVertices.clear();
    mObjectVertices.shrink_to_fit();

    mTextureSurfaces.clear();
    mTextureSurfaces.shrink_to_fit();

    mFontMaps.clear();
    mFontMaps.shrink_to_fit();

    mPhase.store(Phase::Cleanup, std::memory_order_release);
}

void LoadingRecipe::CleanupStructures(ResourcesContext &ctx) {
    if (const Phase currentPhase = mPhase.load(std::memory_order_acquire); currentPhase == Phase::Initial) {
        return;
    }


    OnCleanup(ctx);
    mShaderStrings.clear();
    mShaderStrings.shrink_to_fit();

    mPhase.store(Phase::Initial, std::memory_order_release);
}