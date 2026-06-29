#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Fwd/GLFwd.h"
#include "Engine/Math/Math.h"

class Shader {
public:
    Shader();
    ~Shader();

    [[nodiscard]] bool Build(const std::string& vertexShaderString, const std::string& fragmentShaderString);
    void SetActive() const;

    // Getters
    GLint GetUniformLocation(const std::string& name) const;
    GLuint GetID() const { return mShaderProgram; }

    static void SetVectorUniform (GLint location, const Vector2& vector);
    static void SetVectorUniform (GLint location, const Vector3& vector);
    static void SetVectorUniform (GLint location, const Vector4& vector);
    static void SetMatrixUniform(GLint location, const Matrix4& matrix);
    static void SetFloatUniform(GLint location, float value);
    static void SetTextureUniform(GLint location, int value);
private:
    struct UniformInfo {
        GLint location;
        GLenum type;
    };

    void Destroy();
    void IntrospectUniforms();

    static bool CompileShader(const std::string& shaderString, GLenum shaderType, GLuint& outShaderID);
    static bool IsCompiled(GLuint shader);
    bool IsValidProgram() const;

    GLuint mShaderProgram;

    std::pmr::unordered_map<std::string, UniformInfo> mUniforms;
};