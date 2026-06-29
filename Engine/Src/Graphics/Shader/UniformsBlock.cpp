#include "Engine/Graphics/Shader/UniformsBlock.h"
#include "Engine/Graphics/Shader/Shader.h"

void UniformsBlock::SetFloat(const Shader* shader, const std::string& name, const float value) {
    if (!shader) {
        return;
    }

    const int location = shader->GetUniformLocation(name);
    if (location == -1) {
        return;
    }

    for (auto&[uniformLocation, uniformValue ] : mFloats) {
        if (uniformLocation == location) {
            uniformValue = value;
            return;
        }
    }

    mFloats.emplace_back(location, value);
}


void UniformsBlock::SetVector(const Shader* shader, const std::string &name, const Vector3 &value) {
    if (!shader) {
        return;
    }

    const int location = shader->GetUniformLocation(name);
    if (location == -1) {
        return;
    }

    for (auto&[uniformLocation, uniformValue] : mVectors3) {
        if (uniformLocation == location) {
            uniformValue = value;
            return;
        }
    }

    mVectors3.emplace_back(location, value);
}

void UniformsBlock::SetVector(const Shader* shader, const std::string &name, const Vector4 &value) {
    if (!shader) {
        return;
    }

    const int location = shader->GetUniformLocation(name);
    if (location == -1) {
        return;
    }

    for (auto&[uniformLocation, uniformValue] : mVectors4) {
        if (uniformLocation == location) {
            uniformValue = value;
            return;
        }
    }

    mVectors4.emplace_back(location, value);
}



void UniformsBlock::SetMatrix(const Shader* shader, const std::string &name, const Matrix4 &value) {
    if (!shader) {
        return;
    }

    const int location = shader->GetUniformLocation(name);
    if (location == -1) {
        return;
    }

    for (auto&[uniformLocation, uniformValue] : mMatrices) {
        if (uniformLocation == location) {
            uniformValue = value;
            return;
        }
    }

    mMatrices.emplace_back(location, value);
}


void UniformsBlock::ApplyUniforms() {
    for (const auto&[location, value] : mFloats) {
        Shader::SetFloatUniform(location, value);
    }

    for (const auto &[location, value] : mVectors3) {
        Shader::SetVectorUniform(location, value);
    }

    for (const auto &[location, value] : mVectors4) {
        Shader::SetVectorUniform(location, value);
    }

    for (const auto &[location, value] : mMatrices) {
        Shader::SetMatrixUniform(location, value);
    }
}