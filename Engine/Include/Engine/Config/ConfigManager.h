#pragma once
#include <string_view>
#include <unordered_map>
#include <string>
#include <variant>

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    void SetUnsigned(std::string_view key, unsigned int value);
    void SetInt(std::string_view key, int value);
    void SetFloat(std::string_view key, float value);
    void SetBool(std::string_view key, bool value);
    void SetString(std::string_view key, std::string_view value);

    [[nodiscard]] unsigned int GetUnsigned(std::string_view key, unsigned int defaultValue = 0) const;
    [[nodiscard]] int GetInt(std::string_view key, int defaultValue = 0) const;
    [[nodiscard]] float GetFloat(std::string_view key, float defaultValue = 0.0f) const;
    [[nodiscard]] bool GetBool(std::string_view key, bool defaultValue = false) const;
    [[nodiscard]] std::string GetString(std::string_view key, std::string_view defaultValue = "") const;

private:
    using ConfigValue = std::variant<unsigned int, int, float, bool, std::string>;
    void LoadDefaults();

    std::unordered_map<std::string, ConfigValue> mConfigDatabase;
};
