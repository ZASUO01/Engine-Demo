#pragma once
#include <vector>

class VertexArray {
public:
    struct VertexAttribute {
        unsigned int index;
        int size;
        unsigned int type;
        bool normalized;
        size_t offset;
    };

    VertexArray();
    ~VertexArray();

    [[nodiscard]] bool Build(const float* vertices, unsigned int numVertices,
                const unsigned int* indices, unsigned int numIndices,
                unsigned int stride, std::vector<VertexAttribute> &layout);

    void SetActive() const;
    [[nodiscard]] unsigned int GetNumIndices() const { return mNumIndices; }
    [[nodiscard]] unsigned int GetNumVertices() const { return mNumVertices; }

    [[nodiscard]] unsigned int GetID() const { return mVertexArray; }

private:
    unsigned int mNumVertices{0};
    unsigned int mNumIndices{0};
    unsigned int mStride{0};
    unsigned int mVertexBuffer{0};
    unsigned int mIndexBuffer{0};
    unsigned int mVertexArray{0};
};