#include "Engine/Assets/AssetsManager.h"
#include "Engine/Graphics/Model.h"
#include "Engine/Graphics/Geometry/Mesh.h"
#include "Engine/Graphics/Resources/Font.h"
#include "Engine/Graphics/Resources/Texture.h"
#include "Engine/Graphics/Shader/ShadersManager.h"

AssetsManager::AssetsManager() = default;
AssetsManager::~AssetsManager() = default;

void AssetsManager::UnloadData() {
    UnloadTextures();
    UnloadModels();
    UnloadMeshes();
    UnloadFonts();
}

Texture* AssetsManager::GetTexture(const std::string &name) {
    if (const auto it = mTextures.find(name); it != mTextures.end()) {
        return it->second.get();
    }

    return nullptr;
}

Texture *AssetsManager::AddTexture(const std::string &name) {
    if (const auto it = mTextures.find(name); it != mTextures.end()) {
        mTextures.erase(it);
    }

    auto uniqueTexture = std::make_unique<Texture>();
    mTextures[name] = std::move(uniqueTexture);
    return mTextures[name].get();
}

void AssetsManager::RemoveTexture(const std::string &name) {
    if (const auto it = mTextures.find(name); it != mTextures.end()) {
        mTextures.erase(it);
    }
}

void AssetsManager::UnloadTextures() {
    mTextures.clear();
}

Mesh* AssetsManager::GetMesh(const std::string& name) {
    if (const auto it = mMeshes.find(name); it != mMeshes.end()) {
        return it->second.get();
    }

   return nullptr;
}

Mesh *AssetsManager::AddMesh(const std::string &name) {
    if (const auto it = mMeshes.find(name); it != mMeshes.end()) {
        mMeshes.erase(it);
    }

    auto uniqueMesh = std::make_unique<Mesh>();
    mMeshes[name] = std::move(uniqueMesh);
    return mMeshes[name].get();
}

void AssetsManager::RemoveMesh(const std::string &name) {
    if (const auto it = mMeshes.find(name); it != mMeshes.end()) {
        mMeshes.erase(it);
    }
}

void AssetsManager::UnloadMeshes() {
    mMeshes.clear();
}

Model *AssetsManager::GetModel(const std::string &name) {
    if (const auto it = mModels.find(name); it != mModels.end()) {
        return it->second.get();
    }

    return nullptr;
}

Model* AssetsManager::AddModel(const std::string& name) {
    if (const auto it = mModels.find(name); it != mModels.end()) {
        mModels.erase(it);
    }

    const auto mesh = GetMesh("Engine/Assets/Models/Default");
    const auto texture = GetTexture("Engine/Assets/Textures/Default");

    if (!mesh || !texture) {
        return nullptr;
    }

    auto uniqueModel = std::make_unique<Model>(mesh, texture);
    mModels[name] = std::move(uniqueModel);
    return mModels[name].get();
}

void AssetsManager::RemoveModel(const std::string &name) {
    if (const auto it = mModels.find(name); it != mModels.end()) {
        mModels.erase(it);
    }
}

void AssetsManager::UnloadModels() {
    mModels.clear();
}

Font* AssetsManager::GetFont(const std::string &name) {
    if (const auto it = mFonts.find(name); it != mFonts.end()) {
        return it->second.get();
    }

    return nullptr;
}

Font* AssetsManager::AddFont(const std::string &name) {
    if (const auto it = mFonts.find(name); it != mFonts.end()) {
        mFonts.erase(it);
    }

    auto uniqueFont = std::make_unique<Font>();
    mFonts[name] = std::move(uniqueFont);
    return mFonts[name].get();
}

void AssetsManager::RemoveFont(const std::string &name) {
    if (const auto it = mFonts.find(name); it != mFonts.end()) {
        mFonts.erase(it);
    }
}

void AssetsManager::UnloadFonts() {
    mFonts.clear();
}
