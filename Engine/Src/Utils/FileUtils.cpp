#include "Engine/Utils/FileUtils.h"
#include <fstream>
#include <sstream>
#include <SDL2/SDL_log.h>

bool FileUtils::LoadFileToString(const std::string& filePath, std::string& outString) {
    outString.clear();

    std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        SDL_Log("[FILE UTILS] Failed to open file %s", filePath.c_str());
        return false;
    }

    try {
        const std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        outString.resize(static_cast<size_t>(fileSize));
        file.read(&outString[0], fileSize);

        file.close();
    }catch (std::exception& e) {
        SDL_Log("[FILE UTILS] Failed process file %s: %s", filePath.c_str(), e.what());
        return false;
    }

    return true;
}