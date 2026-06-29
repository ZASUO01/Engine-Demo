#pragma once
#include "Engine/Graphics/PostProcess/PostEffect.h"
#include "Engine/Graphics/Shader/Shader.h"
#include "Engine/Graphics/Shader/UniformsBlock.h"

PostEffect::PostEffect(Shader *shader)
:mShader(shader) {
    mUniformsBlock = std::make_unique<UniformsBlock>();
}

PostEffect::~PostEffect() = default;
