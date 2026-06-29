#include "Engine/Config/ConfigManager.h"

#include <cstdint>

#include "Engine/Config/ConfigFlags.h"

ConfigManager::ConfigManager() {
    LoadDefaults();
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::SetUnsigned(const std::string_view key, unsigned int value) {
    mConfigDatabase[std::string(key)] = value;
}

void ConfigManager::SetInt(const std::string_view key, int value) {
    mConfigDatabase[std::string(key)] = value;
}

void ConfigManager::SetFloat(const std::string_view key, float value) {
    mConfigDatabase[std::string(key)] = value;
}

void ConfigManager::SetBool(const std::string_view key, bool value) {
    mConfigDatabase[std::string(key)] = value;
}

void ConfigManager::SetString(const std::string_view key, const std::string_view value) {
    mConfigDatabase[std::string(key)] = std::string(value);
}

unsigned int ConfigManager::GetUnsigned(const std::string_view key, const unsigned int defaultValue) const {
    if (const auto it = mConfigDatabase.find(std::string(key)); it != mConfigDatabase.end()) {
        if (const unsigned int* val = std::get_if<unsigned int>(&it->second)) {
            return *val;
        }
    }
    return defaultValue;
}

int ConfigManager::GetInt(const std::string_view key, const int defaultValue) const {
    if (const auto it = mConfigDatabase.find(std::string(key)); it != mConfigDatabase.end()) {
        if (const int* val = std::get_if<int>(&it->second)) {
            return *val;
        }
    }
    return defaultValue;
}

float ConfigManager::GetFloat(const std::string_view key, const float defaultValue) const {
    if (const auto it = mConfigDatabase.find(std::string(key)); it != mConfigDatabase.end()) {
        if (const float* val = std::get_if<float>(&it->second)) {
            return *val;
        }
    }
    return defaultValue;
}

bool ConfigManager::GetBool(const std::string_view key, const bool defaultValue) const {
    if (const auto it = mConfigDatabase.find(std::string(key)); it != mConfigDatabase.end()) {
        if (const bool* val = std::get_if<bool>(&it->second)) {
            return *val;
        }
    }
    return defaultValue;
}

std::string ConfigManager::GetString(const std::string_view key, const std::string_view defaultValue) const {
    if (const auto it = mConfigDatabase.find(std::string(key)); it != mConfigDatabase.end()) {
        if (const std::string* val = std::get_if<std::string>(&it->second)) {
            return *val;
        }
    }
    return std::string(defaultValue);
}

void ConfigManager::LoadDefaults() {
    SetString("engine.window.title", "Game Engine");
    SetInt("engine.window.width", 1024);
    SetInt("engine.window.height", 768);

    SetString("engine.shaders.shadow", "Engine/Shaders/Shadow");
    SetString("engine.shaders.models", "Engine/Shaders/Models");
    SetString("engine.shaders.shadowModels", "Engine/Shaders/ShadowModels");
    SetString("engine.shaders.ui", "Engine/Shaders/UI");
    SetString("engine.shaders.debug", "Engine/Shaders/Debug");

    SetString("engine.assets.models.default", "Engine/Assets/Models/Default");
    SetString("engine.assets.textures.default", "Engine/Assets/Textures/Default");
    SetString("engine.assets.fonts.mono", "Engine/Assets/Fonts/JetBrainsMono");

    const auto initialMask = static_cast<uint32_t>(DrawMode::Models | DrawMode::Shadow | DrawMode::PostProcess);
    SetUnsigned("engine.draw.mask", initialMask);
}
