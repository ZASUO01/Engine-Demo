#include "Engine/Core/Components/Drawing/ModelComponent.h"

#include "Engine/Core/Actors/Actor.h"
#include "Engine/Graphics/Model.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Renderer/RenderUtils.h"
#include "Engine/Graphics/Shader/UniformsBlock.h"

ModelComponent::ModelComponent(Actor* actor, Model* model)
:Component(actor)
,mModel(model)
{
    mBlock = std::make_unique<UniformsBlock>();
}

ModelComponent::~ModelComponent() = default;

void ModelComponent::SendDrawCommand(Renderer *renderer, const DrawMode mode) {
    if (!HasFlag(mode, DrawMode::Models)) {
        return;
    }

    const auto shader = mModel->GetShader();
    const auto shaderID = shader ? mModel->GetShaderID() : 0;
    const auto vertexArray = mModel->GetVertexArray();
    const auto vertexArrayID = mModel->GetVertexArrayID();
    const auto texture = mModel->GetTexture(mTextureIndex);
    const auto textureID = mModel->GetTextureID(mTextureIndex);
    const auto transform = GetActor()->GetWorldTransform();

    if (HasFlag(mode, DrawMode::Shadow)) {
        const auto shadowKey = RenderUtils::GenerateRenderKey(0, vertexArrayID, 0, GetDrawOrder());

        RenderUtils::ShadowsCommand shadowCmd = {
            shadowKey,
            vertexArrayID,
            vertexArray,
            transform,
        };

        renderer->PushShadowCommand(shadowCmd);
    }

    const auto key = RenderUtils::GenerateRenderKey(shaderID, vertexArrayID, textureID, GetDrawOrder());

    RenderUtils::ModelCommand cmd = {
        key,
        shaderID,
        vertexArrayID,
        textureID,
        shader,
        vertexArray,
        texture,
        transform,
        mBlock.get()
    };

    renderer->PushModelCommand(cmd);
}

void ModelComponent::SetTextureIndex(const int index) {
    if (index < 0 || index >= mModel->GetTextureCount()) {
        return;
    }

    mTextureIndex = index;
}
