#pragma once
#include <string>
#include <vector>
#include "Engine/Math/Math.h"

class UniformsBlock {
public:
    ~UniformsBlock() = default;

    void SetFloat(const class Shader* shader, const std::string& name, float value);
    void SetVector(const Shader* shader, const std::string& name, const Vector3& value);
    void SetVector(const Shader* shader, const std::string& name, const Vector4& value);
    void SetMatrix(const Shader* shader, const std::string& name, const Matrix4& value);

    void ApplyUniforms();

private:
    // Shader uniforms
    struct FloatUniform { int location; float value; };
    struct Vector3Uniform { int location{}; Vector3 value; };
    struct Vector4Uniform { int location{}; Vector4 value; };
    struct MatrixUniform { int location{}; Matrix4 value; };

    std::vector<FloatUniform>  mFloats;
    std::vector<Vector3Uniform> mVectors3;
    std::vector<Vector4Uniform> mVectors4;
    std::vector<MatrixUniform> mMatrices;
};