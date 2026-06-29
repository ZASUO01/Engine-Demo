#include "Engine/Graphics/Geometry/VertexArray.h"
#include <SDL2/SDL_log.h>
#include  <glad/glad.h>

VertexArray::VertexArray() = default;

VertexArray::~VertexArray() {
    if (mVertexBuffer != 0) glDeleteBuffers(1, &mVertexBuffer);
    if (mIndexBuffer != 0) glDeleteBuffers(1, &mIndexBuffer);
    if (mVertexArray != 0) glDeleteVertexArrays(1, &mVertexArray);
}

bool VertexArray::Build(const float *vertices, const unsigned int numVertices, const unsigned int *indices,
                         const unsigned int numIndices, const unsigned int stride,
                         std::vector<VertexAttribute> &layout)
{
    mNumVertices = numVertices;
    mNumIndices = numIndices;
    mStride = stride;

    // cleans previous errors
    glGetError();

    // Create vertex array
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    // Create vertex buffer
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * stride, vertices, GL_STATIC_DRAW);

    // Create index buffer
    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(numIndices * sizeof(GLuint)), indices, GL_STATIC_DRAW);

    for (const auto&[index, size, type, normalized, offset] : layout){
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(
            index,
            size,
            type,
            normalized ? GL_TRUE : GL_FALSE,
            static_cast<GLsizei>(stride),
            reinterpret_cast<void*>(offset)
        );
    }

    if (glGetError() != GL_NO_ERROR) {
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mIndexBuffer);
        glDeleteVertexArrays(1, &mVertexArray);
        mVertexBuffer = 0;
        mIndexBuffer = 0;
        mVertexArray = 0;

        SDL_Log("[VERTEX ARRAY] Failed to creat vertex array");
        return false;
    }

    glBindVertexArray(0);
    return true;
}

void VertexArray::SetActive() const {
    glBindVertexArray(mVertexArray);
}