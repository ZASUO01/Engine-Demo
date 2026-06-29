#include "Engine/Graphics/Renderer/Drawer.h"
#include "Engine/Graphics/Renderer/RenderUtils.h"
#include <algorithm>
#include <SDL2/SDL_log.h>
#include <glad/glad.h>
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Graphics/Geometry/VertexArray.h"
#include "Engine/Graphics/PostProcess/PostEffect.h"
#include "Engine/Graphics/Resources/Texture.h"
#include "Engine/Graphics/Shader/UniformsBlock.h"

Drawer::Drawer()
:mShadowShader(nullptr)
,mModelsShader(nullptr)
,mShadowModelsShader(nullptr)
,mSpritesShader(nullptr)
,mDebugShader(nullptr)
{}

Drawer::~Drawer() = default;

bool Drawer::Init() {
    if (!GenerateVertices()) {
        return false;
    }

    return true;
}

void Drawer::DrawShadowPass(std::vector<RenderUtils::ShadowsCommand> &shadowQueue, const Matrix4& lightViewProjectionMatrix) const {
    if (shadowQueue.empty() || !mShadowShader) {
        shadowQueue.clear();
        return;
    }

    std::ranges::sort(shadowQueue, [](const auto &a, const auto &b) {
        return a.key < b.key;
    });

    mShadowShader->SetActive();

    if (const int loc = mShadowShader->GetUniformLocation("uLightSpace"); loc >= 0) {
        Shader::SetMatrixUniform(loc, lightViewProjectionMatrix);
    }

    const int transformLoc = mShadowShader->GetUniformLocation("uTransform");

    unsigned int lastVertexArrayID = 0;
    const VertexArray* vertexArray = nullptr;
    GLsizei numIndices = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    for (const auto& cmd : shadowQueue) {
        if (!cmd.vertexArray) continue;

        if (cmd.vertexArrayID != lastVertexArrayID) {
            vertexArray = cmd.vertexArray;
            vertexArray->SetActive();
            numIndices = static_cast<GLsizei>(vertexArray->GetNumIndices());
            lastVertexArrayID = cmd.vertexArrayID;
        }

        if (transformLoc >= 0) {
            Shader::SetMatrixUniform(transformLoc, cmd.transform);
        }

        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
    }
    glCullFace(GL_BACK);
    shadowQueue.clear();
}

void Drawer::DrawModelPass(std::vector<RenderUtils::ModelCommand> &modelQueue, const bool applyShadows, const ModelPassData& data) const {
    if (modelQueue.empty() || !mModelsShader || !mShadowModelsShader) {
        return;
    }

    std::ranges::sort(modelQueue, [](const auto &a, const auto &b) {
        return a.key < b.key;
    });

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    unsigned int lastShaderID = 0;
    const unsigned int shadowModelsShaderID =  mShadowModelsShader->GetID();
    const unsigned int modelsShaderID = mModelsShader->GetID();
    const Shader* shader = nullptr;
    bool shaderChanged = false;

    unsigned int lastVertexArrayID = 0;
    const VertexArray* vertexArray = nullptr;
    GLsizei numIndices = 0;

    unsigned int lastTextureID = 0;
    const Texture* texture = nullptr;

    for (const auto&cmd : modelQueue) {
        if (!cmd.vertexArray || !cmd.texture) {
            continue;
        }

        if (!cmd.shader) {
            if (applyShadows &&  shadowModelsShaderID != lastShaderID) {
                shader = mShadowModelsShader;
                shader->SetActive();
                shaderChanged = true;

                if (const int lsLoc = shader->GetUniformLocation("uLightSpace"); lsLoc >= 0) {
                    Shader::SetMatrixUniform(lsLoc, data.lightViewProjectionMatrix);
                }

                if (const int dirLoc = shader->GetUniformLocation("uLightDir"); dirLoc >= 0) {
                    Shader::SetVectorUniform(dirLoc, data.lightDir);
                }

                if (const int shadowLoc = shader->GetUniformLocation("uShadowMap"); shadowLoc >= 0) {
                    Shader::SetTextureUniform(shadowLoc, data.shadowTextureSlot);
                }

                lastShaderID = shadowModelsShaderID;
            }
            else if (!applyShadows && modelsShaderID != lastShaderID) {
                shader = mModelsShader;
                shader->SetActive();
                shaderChanged = true;
                lastShaderID = modelsShaderID;
            }
        }else {
            if (cmd.shaderID != lastShaderID) {
                shader = cmd.shader;
                shader->SetActive();
                shaderChanged = true;
                lastShaderID = cmd.shaderID;
            }
        }

        if (shaderChanged) {
            if (const int texLocation = shader->GetUniformLocation("uTexture"); texLocation >= 0) {
                Shader::SetTextureUniform(texLocation, 0);
            }

            if (const int viewProjLocation = shader->GetUniformLocation("uViewProjection"); viewProjLocation >= 0) {
                Shader::SetMatrixUniform(viewProjLocation, data.viewProjectionMatrix);
            }

            lastVertexArrayID = 0;
            lastTextureID = 0;
            shaderChanged = false;
        }

        if (shader) {
            if (const int transformLoc = shader->GetUniformLocation("uTransform"); transformLoc >= 0) {
                Shader::SetMatrixUniform(transformLoc, cmd.transform);
            }
        }

        if (cmd.vertexArrayID != lastVertexArrayID) {
            vertexArray = cmd.vertexArray;
            vertexArray->SetActive();
            numIndices = static_cast<GLsizei>(vertexArray->GetNumIndices());
            lastVertexArrayID = cmd.vertexArrayID;
        }

        if (cmd.textureID != lastTextureID) {
            texture = cmd.texture;

            if (cmd.textureID != 0) {
                texture->SetActive();
            }else {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            lastTextureID = cmd.textureID;
        }

        cmd.uniformsBlock->ApplyUniforms();
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    modelQueue.clear();
}

void Drawer::DrawSpritesPass(std::vector<RenderUtils::SpriteCommand> &sprites, const Matrix4 &orthoMatrix) const {
    if (sprites.empty() || !mSpritesShader || !mSpriteVertices) {
        return;
    }

    std::ranges::sort(sprites, [](const auto&a , const auto& b) {
        return a.key < b.key;
    });

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const unsigned int spriteShaderID = mSpritesShader->GetID();
    bool shaderChanged = false;
    uint32_t lastShaderID = 0;
    const Shader* shader = nullptr;

    int transformLocation = -1;
    int textureRectLocation = -1;
    int colorLocation = -1;

    mSpriteVertices->SetActive();
    const auto numIndices = mSpriteVertices->GetNumIndices();

    uint32_t lastTextureID = 0;
    const Texture* texture = nullptr;

    for (auto& cmd: sprites) {

        if (!cmd.shader) {
            if (spriteShaderID != lastShaderID) {
                shader = mSpritesShader;
                shader->SetActive();
                shaderChanged = true;

                lastShaderID = spriteShaderID;
            }

        }else {
            if (cmd.shaderID != lastShaderID) {
                shader = cmd.shader;
                shader->SetActive();
                shaderChanged = true;

                lastShaderID = cmd.shaderID;
            }
        }

        if (shaderChanged) {
            transformLocation = shader->GetUniformLocation("uTransform");
            textureRectLocation = shader->GetUniformLocation("uTextureRect");
            colorLocation = shader->GetUniformLocation("uColor");

            if (const int orthoLocation = shader->GetUniformLocation("uOrtho"); orthoLocation >= 0) {
                Shader::SetMatrixUniform(orthoLocation, orthoMatrix);
            }

            if (const int texLocation = shader->GetUniformLocation("uTexture"); texLocation >= 0) {
                Shader::SetTextureUniform(texLocation, 0);
            }

            if (const int textureFactorLocation = shader->GetUniformLocation("uTextureFactor"); textureFactorLocation >= 0) {
                Shader::SetFloatUniform(textureFactorLocation, 1.0f);
            }

            shaderChanged = false;
            lastTextureID = 0;
        }

        if (transformLocation >= 0) {
            Shader::SetMatrixUniform(transformLocation, cmd.transform);
        }

        if (textureRectLocation >= 0) {
            Shader::SetVectorUniform(textureRectLocation, cmd.textureRect);
        }

        if (colorLocation >= 0) {
            Shader::SetVectorUniform(colorLocation, cmd.color);
        }

        if (cmd.textureID != lastTextureID) {
            texture = cmd.texture;

            // active the texture if it exists
            if (texture && cmd.textureID != 0) {
                texture->SetActive();
            }
            // deactivate the previous one otherwise
            else {
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            lastTextureID = cmd.textureID;
        }

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices), GL_UNSIGNED_INT, nullptr);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    sprites.clear();
}

void Drawer::DrawDebugPass(std::vector<RenderUtils::DebugCommand> &debugCommands, const Matrix4 &viewProjectionMatrix) const {
    if (debugCommands.empty() || !mDebugShader) {
        return;
    }

    std::ranges::sort(debugCommands, [](const auto &a, const auto &b) {
        return a.key < b.key;
    });

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    mDebugShader->SetActive();
    unsigned int previousIsTrigger = false;
    const int viewProjLocation = mDebugShader->GetUniformLocation("uViewProjection");
    const int transformLocation = mDebugShader->GetUniformLocation("uTransform");
    const int colorLocation = mDebugShader->GetUniformLocation("uColor");

    Shader::SetMatrixUniform(viewProjLocation, viewProjectionMatrix);
    Shader::SetVectorUniform(colorLocation, Color::Green);

    auto lastType = RenderUtils::DebugType::None;
    GLsizei numIndices = 0;

    for (const auto& cmd : debugCommands) {
        if (cmd.type == RenderUtils::DebugType::None) {
            continue;
        }

        if (cmd.isTrigger != previousIsTrigger) {
            if (cmd.isTrigger) {
                Shader::SetVectorUniform(colorLocation, Color::Yellow);
            }else {
                Shader::SetVectorUniform(colorLocation, Color::Green);
            }

            previousIsTrigger = cmd.isTrigger;
        }

        if (cmd.type != lastType) {
            switch (cmd.type) {
                case RenderUtils::DebugType::ColliderPLane:
                    mPlaneDebugVertices->SetActive();
                    numIndices = static_cast<GLsizei>(mPlaneDebugVertices->GetNumIndices());
                    break;
                case RenderUtils::DebugType::ColliderSphere:
                    mSphereDebugVertices->SetActive();
                    numIndices = static_cast<GLsizei>(mSphereDebugVertices->GetNumIndices());
                    break;
                case RenderUtils::DebugType::ColliderBox:
                    mBoxDebugVertices->SetActive();
                    numIndices = static_cast<GLsizei>(mBoxDebugVertices->GetNumIndices());
                    break;
                default:
                    numIndices = 0;
                    break;
            }
            lastType = cmd.type;
        }

        if (numIndices == 0) {continue;}

        Shader::SetMatrixUniform(transformLocation, cmd.transform);
        glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, nullptr);
    }

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    debugCommands.clear();
}

void Drawer::DrawPostPass(const PostEffect* effect, const int screenTextureSlot) const {
    if (!mFullScreenQuad || !effect) {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    const auto shader = effect->GetShader();
    shader->SetActive();

    if (const auto textureLoc = shader->GetUniformLocation("uScreenTexture"); textureLoc >= 0) {
        Shader::SetTextureUniform(textureLoc, screenTextureSlot);
    }

    mFullScreenQuad->SetActive();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mFullScreenQuad->GetNumIndices()), GL_UNSIGNED_INT, nullptr);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

bool Drawer::GenerateVertices() {
    constexpr unsigned int quadStride = 4 * sizeof(float);

    std::vector<float> quadVertices;
    std::vector<unsigned int> quadIndices;

    GenerateFullScreenQuad(quadVertices, quadIndices);

    std::vector<VertexArray::VertexAttribute> quadLayout;
    quadLayout.emplace_back(0, 2, GL_FLOAT, false, 0);
    quadLayout.emplace_back(1, 2, GL_FLOAT, false, 2 * sizeof(float));

    mFullScreenQuad = std::make_unique<VertexArray>();
    if (!mFullScreenQuad->Build(quadVertices.data(), quadVertices.size() / 4, quadIndices.data(), quadIndices.size(),
        quadStride, quadLayout)) {
        SDL_Log("[DRAWER] Failed to build full quad VertexArray");
        return false;
    }

    std::vector<float> spriteVertices;
    std::vector<unsigned int> spriteIndices;

    GenerateSpriteVertices(spriteVertices, spriteIndices);

    mSpriteVertices = std::make_unique<VertexArray>();
    if (!mSpriteVertices->Build(spriteVertices.data(), spriteVertices.size() / 4, spriteIndices.data(), spriteIndices.size(), quadStride, quadLayout)) {
        SDL_Log("[DRAWER] Failed to build sprite VertexArray");
        return false;
    }

    constexpr unsigned int stride = 3 * sizeof(float); // 3 x 4 bytes;

    std::vector<VertexArray::VertexAttribute> layout;
    layout.emplace_back(0, 3, GL_FLOAT, false, 0);

    std::vector<float> planeVertices;
    std::vector<unsigned int> planeIndices;

    GeneratePlaneVertices(planeVertices, planeIndices);

    mPlaneDebugVertices = std::make_unique<VertexArray>();
    if (!mPlaneDebugVertices->Build(planeVertices.data(), planeVertices.size() / 3, planeIndices.data(), planeIndices.size(),
        stride, layout)) {
        SDL_Log("[DRAWER] Failed to build plane VertexArray");
        return false;
        }

    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    GenerateSphereVertices(sphereVertices, sphereIndices);

    mSphereDebugVertices = std::make_unique<VertexArray>();
    if (!mSphereDebugVertices->Build(sphereVertices.data(), sphereVertices.size() / 3, sphereIndices.data(),
                                     sphereIndices.size(), stride, layout)) {
        SDL_Log("[DRAWER] Failed to build sphere VertexArray");
        return false;
    }

    std::vector<float> boxVertices;
    std::vector<unsigned int> boxIndices;

    GenerateBoxVertices(boxVertices, boxIndices);

    mBoxDebugVertices = std::make_unique<VertexArray>();
    if (!mBoxDebugVertices->Build(boxVertices.data(), boxVertices.size() / 3, boxIndices.data(),
                                     boxIndices.size(), stride, layout)) {
        SDL_Log("[DRAWER] Failed to build box VertexArray");
        return false;
    }

    return true;
}

void Drawer::GenerateFullScreenQuad(std::vector<float> &vertices, std::vector<unsigned int> &indices) {
    constexpr float quadVertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         -1.0f, 1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        1.0f,   -1.0f, 1.0f, 0.0f,
    };

    const unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    vertices.assign(quadVertices, quadVertices + 16);
    indices.assign(quadIndices, quadIndices + 6);
}

void Drawer::GenerateSpriteVertices(std::vector<float> &vertices, std::vector<unsigned int> &indices) {
    constexpr float spriteVertices[] = {
        -0.5f,  0.5f, 0.0f, 1.0f,
         -0.5f, -0.5f, 0.0f, 0.0f,
          0.5f, -0.5f, 1.0f, 0.0f,
          0.5f,  0.5f, 1.0f, 1.0f
        };

    const unsigned int spriteIndices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    vertices.assign(spriteVertices, spriteVertices + 16);
    indices.assign(spriteIndices, spriteIndices + 6);
}

void Drawer::GeneratePlaneVertices(std::vector<float> &vertices, std::vector<unsigned int> &indices) {
    constexpr float planeVertices[] = {
        -1.0f,  0.0f, -1.0f,
         1.0f,  0.0f, -1.0f,
         1.0f,  0.0f,  1.0f,
        -1.0f,  0.0f,  1.0f
    };

    const unsigned int planeIndices[] = {
        0, 1,
        1, 2,
        2, 3,
        3, 0
    };

    vertices.assign(planeVertices, planeVertices + 12);
    indices.assign(planeIndices, planeIndices + 8);
}

void Drawer::GenerateSphereVertices(std::vector<float> &vertices, std::vector<unsigned int> &indices) {
    constexpr int segments = 24;
    constexpr float angleStep = Math::TwoPi / segments;

    for (int i = 0; i < segments; ++i) {

    }

    auto buildRing = [&](const int axis) {
        const auto startIndex = static_cast<uint32_t>(vertices.size() / 3);

        for (int i = 0; i < segments; ++i) {
            const auto angle = static_cast<float>(i) * angleStep;
            const float c = Math::Cos(angle);
            const float s = Math::Sin(angle);

            if (axis == 0) {
                vertices.emplace_back(c);
                vertices.emplace_back(s);
                vertices.emplace_back(0.0f);
            } else if (axis == 1) {
                vertices.emplace_back(c);
                vertices.emplace_back(0.0f);
                vertices.emplace_back(s);
            } else {
                vertices.emplace_back(0.0f);
                vertices.emplace_back(c);
                vertices.emplace_back(s);
            }

            indices.emplace_back(startIndex + i);
            indices.emplace_back(startIndex + (i + 1) % segments);
        }
    };

    buildRing(0);
    buildRing(1);
    buildRing(2);
}

void Drawer::GenerateBoxVertices(std::vector<float> &vertices, std::vector<unsigned int> &indices) {
    constexpr float v[] = {
        -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f
    };

    vertices.assign(v, v + 24);

    const unsigned int i[] = {
        0, 1,  1, 2,  2, 3,  3, 0,
        4, 5,  5, 6,  6, 7,  7, 4,
        0, 4,  1, 5,  2, 6,  3, 7
    };

    indices.assign(i, i + 24);
}
