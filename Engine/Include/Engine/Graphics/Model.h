#pragma once
#include <vector>

class Shader;
class Mesh;
class VertexArray;
class Texture;

class Model {
public:
    explicit Model(Mesh* baseMesh, Texture* baseTexture);
    ~Model();

    void SetShader(Shader* shader) { mShader = shader; }
    [[nodiscard]] Shader* GetShader() const { return mShader; }
    [[nodiscard]] unsigned int GetShaderID() const;

    void SetMesh(Mesh* mesh) { mMesh = mesh; }
    [[nodiscard]] Mesh* GetMesh() const { return mMesh; }

    [[nodiscard]] VertexArray* GetVertexArray() const;
    [[nodiscard]] unsigned int GetVertexArrayID() const;

    void AddTexture(Texture* texture) { if (texture) { mTextures.emplace_back(texture); } }
    [[nodiscard]] Texture* GetTexture(int index) const;
    [[nodiscard]] unsigned int GetTextureID(int index) const;
    [[nodiscard]] unsigned int GetTextureCount() const { return mTextures.size(); }
private:
    Shader* mShader;
    Mesh* mMesh;
    std::vector<Texture*> mTextures;
};