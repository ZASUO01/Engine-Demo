#pragma once
#include <memory>
#include "Engine/Core/Components/Component.h"

class Model;
class Texture;
class UniformsBlock;

class ModelComponent : public Component{
public:
    explicit ModelComponent(Actor* actor, Model* model);
    ~ModelComponent() override;

    void SendDrawCommand(Renderer *renderer, DrawMode mode) override;

    [[nodiscard]] UniformsBlock *GetUniformsBlock() const { return  mBlock.get(); }
    void SetTextureIndex(int index);
private:
    int mTextureIndex{0};
    Model* mModel;
    std::unique_ptr<UniformsBlock> mBlock;
};
