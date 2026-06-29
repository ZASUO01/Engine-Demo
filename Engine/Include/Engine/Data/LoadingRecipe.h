#pragma once
#include <stop_token>
#include <unordered_map>
#include <vector>

#include "Engine/Fwd/SDLFwd.h"

struct ResourcesContext;

class LoadingRecipe {
public:
    virtual ~LoadingRecipe();
    [[nodiscard]] float GetProgress() const { return mProgress.load(std::memory_order_relaxed); }

protected:
    virtual void OnLoad(class ConfigManager &manager) = 0;
    virtual void OnBuild(ResourcesContext& ctx) = 0;
    virtual void OnCleanup(ResourcesContext& ctx) = 0;

    void LoadShaderStrings(const std::string& name);
    void LoadObjectVertices(const std::string& name);
    void LoadTextureSurface(const std::string& name);
    void LoadTTFFont(const std::string& name);

    void UpdateProgress(const float progress) { mProgress.store(progress, std::memory_order_relaxed); }
private:
    enum class Phase {
        Initial,
        Load,
        Build,
        Cleanup
    };

    struct ShaderStrings {
        std::string vertexString;
        std::string fragmentString;
        std::string name;
    };

    struct ObjectVertices {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::string name;
    };

    struct TextureSurface {
        UniqueSurface surface;
        std::string name;
    };

    struct FontMap {
        std::unordered_map<int, UniqueTTFFont> fontMap;
        std::string name;
    };

    void LoadData(const std::stop_token &st, ConfigManager& configManager);
    void BuildStructures(ResourcesContext& ctx);
    void CleanupStructures(ResourcesContext& ctx);
    [[nodiscard]] bool StopRequested() const { return mStopToken.stop_requested(); }

    std::vector<ShaderStrings> mShaderStrings;
    std::vector<ObjectVertices> mObjectVertices;
    std::vector<TextureSurface> mTextureSurfaces;
    std::vector<FontMap> mFontMaps;

    std::atomic<Phase> mPhase{Phase::Initial};
    std::atomic<float> mProgress;
    std::stop_token mStopToken;

    friend class ResourcesLoader;
};
