#include "Engine/Graphics/Shader/ShadersManager.h"
#include "Engine/Graphics/Shader/Shader.h"

ShadersManager::ShadersManager()  = default;

ShadersManager::~ShadersManager() {
    UnloadShaders();
}

Shader *ShadersManager::GetShader(const std::string &name) {
    if (const auto it = mShaders.find(name); it != mShaders.end()) {
        return it->second.get();
    }

    return nullptr;
}

Shader* ShadersManager::AddShader(const std::string& name) {
    if (const auto it = mShaders.find(name); it != mShaders.end()) {
        mShaders.erase(it);
    }

    auto uniqueShader = std::make_unique<Shader>();
    mShaders[name] = std::move(uniqueShader);

    if (mPendingShaderListeners.contains(name)) {
        for (auto& callback : mPendingShaderListeners[name]) {
            callback(mShaders[name].get());
        }
        mPendingShaderListeners.erase(name);
    }

    return mShaders[name].get();
}

void ShadersManager::RemoveShader(const std::string &name) {
    if (const auto it = mShaders.find(name); it != mShaders.end()) {
        mShaders.erase(it);
    }
}

void ShadersManager::UnloadShaders() {
    mShaders.clear();
}

void ShadersManager::ListenForShader(const std::string &name, const std::function<void(Shader *)> &callback) {
    if (mShaders.contains(name)) {
        callback(mShaders[name].get());
        return;
    }

    mPendingShaderListeners[name].emplace_back(callback);
}