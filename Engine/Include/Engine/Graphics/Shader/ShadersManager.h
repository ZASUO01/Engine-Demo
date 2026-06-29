#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class Shader;

class ShadersManager {
public:
    ShadersManager();
    ~ShadersManager();

    [[nodiscard]] Shader* GetShader(const std::string& name);
    Shader* AddShader(const std::string& name);
    void RemoveShader(const std::string& name);
    void UnloadShaders();

    void ListenForShader(const std::string&name, const std::function<void(Shader*)>& callback);
private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders;
    std::unordered_map<std::string, std::vector<std::function<void(Shader*)>>> mPendingShaderListeners;
};
