#pragma once
#include <memory>
#include <string>
#include <unordered_map>

class Texture;
class Mesh;
class Model;
class Font;

class AssetsManager {
public:
    AssetsManager();
    ~AssetsManager();

    void UnloadData();

    [[nodiscard]] Texture* GetTexture(const std::string& name);
    Texture* AddTexture(const std::string& name);
    void RemoveTexture(const std::string& name);
    void UnloadTextures();

    [[nodiscard]] Mesh* GetMesh(const std::string& name);
    Mesh* AddMesh(const std::string& name);
    void RemoveMesh(const std::string& name);
    void UnloadMeshes();

    [[nodiscard]] Model* GetModel(const std::string& name);
    Model* AddModel(const std::string& name);
    void RemoveModel(const std::string& name);
    void UnloadModels();

    [[nodiscard]] Font* GetFont(const std::string& name);
    Font* AddFont(const std::string& name);
    void RemoveFont(const std::string& name);
    void UnloadFonts();

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
    std::unordered_map<std::string, std::unique_ptr<Mesh>> mMeshes;
    std::unordered_map<std::string, std::unique_ptr<Model>> mModels;
    std::unordered_map<std::string, std::unique_ptr<Font>> mFonts;
};