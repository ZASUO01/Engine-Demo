#pragma once
#include <string>
#include <vector>

namespace  ObjectUtils {
    [[nodiscard]] bool LoadFromFile(const std::string &fileName, std::vector<float> &outVertices, std::vector<unsigned int> &outIndices);
}
