#pragma once
#include <memory>

class Shader;
class UniformsBlock;

class PostEffect {
public:
    explicit PostEffect(Shader* shader);
    ~PostEffect();

    [[nodiscard]] Shader* GetShader() const {return mShader; }
    [[nodiscard]] UniformsBlock* GetUniformsBlock() const {return mUniformsBlock.get(); }
private:
    Shader* mShader{nullptr};
    std::unique_ptr<UniformsBlock> mUniformsBlock{nullptr};
};