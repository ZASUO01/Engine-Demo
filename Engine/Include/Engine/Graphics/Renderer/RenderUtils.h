#pragma once
#include <cstdint>
#include "Engine/Math/Math.h"

class Shader;
class VertexArray;
class Texture;
class UniformsBlock;

namespace RenderUtils {
    struct ShadowsCommand {
        uint64_t key{0};
        uint32_t vertexArrayID{0};
        VertexArray *vertexArray{nullptr};
        Matrix4 transform{Matrix4::Identity};
    };

    struct ModelCommand {
        uint64_t key{0};
        uint32_t shaderID{0};
        uint32_t vertexArrayID{0};
        uint32_t textureID{0};
        Shader *shader{nullptr};
        VertexArray *vertexArray{nullptr};
        Texture *texture{nullptr};
        Matrix4 transform{Matrix4::Identity};
        UniformsBlock *uniformsBlock{nullptr};
    };

    struct SpriteCommand {
        uint64_t key{0};
        uint32_t shaderID{0};
        uint32_t textureID{0};
        Shader *shader{nullptr};
        Texture *texture{nullptr};
        Matrix4 transform{Matrix4::Identity};
        Vector4 textureRect{Vector4::Zero};
        Vector3 color{Vector3::Zero};
    };

    enum class DebugType {
        ColliderPLane,
        ColliderSphere,
        ColliderBox,
        None
    };

    struct DebugCommand {
        uint64_t key{0};
        DebugType type{DebugType::None};
        Matrix4 transform{Matrix4::Identity};
        bool isTrigger{false};
    };

    inline uint64_t GenerateRenderKey(const uint32_t shaderID, const uint32_t vaID, const uint32_t textureID, const uint32_t specialOrder = 0) {
        uint64_t renderKey = 0;

        renderKey |= static_cast<unsigned long int>(shaderID & 0xFFFF) << 48;
        renderKey |= static_cast<unsigned long int>(vaID & 0xFFFF) << 32;
        renderKey |= static_cast<unsigned long int>(textureID & 0xFFFF) << 16;
        renderKey |= static_cast<unsigned long int>(specialOrder & 0xFFFF);

        return renderKey;
    }
}
