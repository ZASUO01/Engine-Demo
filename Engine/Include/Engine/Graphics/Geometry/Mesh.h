#pragma once
#include <memory>
#include <vector>

class VertexArray;

class Mesh {
public:
    Mesh();
    ~Mesh();

    [[nodiscard]] bool Build(const std::vector<float> &vertices, const std::vector<unsigned int> &indices);
    [[nodiscard]] VertexArray* GetVertexArray() const { return mVertexArray.get(); }
    [[nodiscard]] unsigned int GetVertexArrayID() const;

private:
    std::unique_ptr<VertexArray> mVertexArray{nullptr};
};