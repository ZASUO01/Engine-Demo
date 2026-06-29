#include "Engine/Graphics/Geometry/Mesh.h"
#include <SDL2/SDL_log.h>
#include "Engine/Graphics/Geometry/VertexArray.h"
#include <glad/glad.h>

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

bool Mesh::Build(const std::vector<float> &vertices, const std::vector<unsigned int> &indices) {
    constexpr unsigned int stride = 8 * sizeof(float); // 8 x 4 bytes;

    std::vector<VertexArray::VertexAttribute> layout;
    layout.emplace_back(0, 3, GL_FLOAT, false, 0);
    layout.emplace_back(1, 3, GL_FLOAT, false, 3 * sizeof(float));
    layout.emplace_back(2, 2, GL_FLOAT, false, 6 * sizeof(float));

    mVertexArray = std::make_unique<VertexArray>();
    if (!mVertexArray->Build(vertices.data(), vertices.size() / 8, indices.data(), indices.size(), stride, layout)) {
        SDL_Log("[Mesh] Failed to build VertexArray");
        return false;
    }

    return  true;
}

unsigned int Mesh::GetVertexArrayID() const {
    return mVertexArray ? mVertexArray->GetID() : 0;
}


