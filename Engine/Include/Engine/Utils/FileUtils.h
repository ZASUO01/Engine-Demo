#pragma once
#include <string>

namespace  FileUtils {
    [[nodiscard]] bool LoadFileToString(const std::string& filePath, std::string& outString);
};
