#pragma once
#include <memory>
#include <vector>

#include "RenderUtils.h"
#include "Engine/Math/Math.h"

namespace RenderUtils {
    struct ModelCommand;
    struct DebugCommand;
}

class Shader;
class VertexArray;
class Texture;
class PostEffect;

class Drawer {
public:
    Drawer();
    ~Drawer();

    bool Init();

    struct ModelPassData {
        Matrix4 viewProjectionMatrix;
        Matrix4 lightViewProjectionMatrix;
        Vector3 lightDir;
        int shadowTextureSlot{0};
    };

    void DrawShadowPass(std::vector<RenderUtils::ShadowsCommand>& shadowQueue, const Matrix4& lightViewProjectionMatrix) const;
    void DrawModelPass(std::vector<RenderUtils::ModelCommand>& modelQueue, bool applyShadows, const ModelPassData& data) const;
    void DrawSpritesPass(std::vector<RenderUtils::SpriteCommand>& sprites, const Matrix4& orthoMatrix) const;
    void DrawDebugPass(std::vector<RenderUtils::DebugCommand>& debugCommands, const Matrix4& viewProjectionMatrix) const;
    void DrawPostPass(const PostEffect *effect, int screenTextureSlot) const;

    void SetShadowShader(Shader* shader){ if (shader){ mShadowShader = shader; } }
    void SetModelsShader(Shader* shader){ if (shader){ mModelsShader = shader; } }
    void SetShadowModelsShader(Shader* shader){ if (shader){ mShadowModelsShader = shader; } }
    void SetUIShader(Shader* shader){ if (shader){ mSpritesShader = shader; } }
    void SetDebugShader(Shader* shader) { if (shader) { mDebugShader = shader; } }
private:
    bool GenerateVertices();

    static void GenerateFullScreenQuad(std::vector<float>& vertices, std::vector<unsigned int>& indices);
    static void GenerateSpriteVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices);
    static void GeneratePlaneVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices);
    static void GenerateSphereVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices);
    static void GenerateBoxVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices);

    Shader* mShadowShader;
    Shader* mModelsShader;
    Shader* mShadowModelsShader;
    Shader* mSpritesShader;
    Shader* mDebugShader;

    std::unique_ptr<VertexArray> mFullScreenQuad{nullptr};
    std::unique_ptr<VertexArray> mSpriteVertices{nullptr};
    std::unique_ptr<VertexArray> mPlaneDebugVertices{nullptr};
    std::unique_ptr<VertexArray> mSphereDebugVertices{nullptr};
    std::unique_ptr<VertexArray> mBoxDebugVertices{nullptr};
};
