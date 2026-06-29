#include "Engine/Graphics/Shader/Shader.h"
#include <SDL2/SDL_log.h>
#include <glad/glad.h>
#include <vector>

Shader::Shader()
:mShaderProgram(0)
{}

Shader::~Shader() {
    Destroy();
}

bool Shader::Build(const std::string &vertexShaderString, const std::string &fragmentShaderString) {
    GLuint vertexShaderID;
    GLuint fragmentShaderID;

    if (!CompileShader(vertexShaderString, GL_VERTEX_SHADER, vertexShaderID) ||
        !CompileShader(fragmentShaderString, GL_FRAGMENT_SHADER, fragmentShaderID)) {
        return false;
    }

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShaderID);
    glAttachShader(mShaderProgram, fragmentShaderID);
    glLinkProgram(mShaderProgram);

    if (!IsValidProgram()) {
        return false;
    }

    IntrospectUniforms();
    return true;
}

void Shader::SetActive() const {
    glUseProgram(mShaderProgram);
}

GLint Shader::GetUniformLocation(const std::string &name) const {
    if (const auto uniformInfo = mUniforms.find(name); uniformInfo != mUniforms.end()) {
        return uniformInfo->second.location;
    }

    return -1;
}

void Shader::SetVectorUniform (const GLint location, const Vector2& vector) {
    glUniform2fv(location, 1, vector.GetAsFloatPtr());
}

void Shader::SetVectorUniform (const GLint location, const Vector3& vector) {
    glUniform3fv(location, 1, vector.GetAsFloatPtr());
}


void Shader::SetVectorUniform (const GLint location, const Vector4& vector) {
    glUniform4fv(location, 1, vector.GetAsFloatPtr());
}


void Shader::SetMatrixUniform(const GLint location, const Matrix4& matrix){
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.GetAsFloatPtr());
}


void Shader::SetFloatUniform(const GLint location, const float value) {
    glUniform1f(location, value);
}


void Shader::SetTextureUniform(const GLint location, const int value) {
    glUniform1i(location, value);
}

void Shader::Destroy() {
    if (mShaderProgram != 0) {
        glDeleteProgram(mShaderProgram);
        mShaderProgram = 0;
    }
    mUniforms.clear();
}

void Shader::IntrospectUniforms() {
    mUniforms.clear();

    GLint numUniforms = 0;
    glGetProgramiv(mShaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

    GLint maxNameLength = 0;
    glGetProgramiv(mShaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

    std::vector<GLchar> nameBuffer(maxNameLength);

    for (GLint i = 0; i < numUniforms; ++i) {
        GLsizei length;
        GLint size;
        GLenum type;

        glGetActiveUniform(mShaderProgram, i, static_cast<int>(nameBuffer.size()), &length, &size, &type, nameBuffer.data());

        std::string name(nameBuffer.data(), length);

        const GLint location = glGetUniformLocation(mShaderProgram, name.c_str());

        mUniforms[name] = { location, type };
    }
}

bool Shader::CompileShader(const std::string &shaderString, const GLenum shaderType, GLuint &outShaderID) {
    const char* contentsChar = shaderString.c_str();

    // Create a shader of the specified type
    outShaderID = glCreateShader(shaderType);

    // Set the source characters and try to compile
    glShaderSource(outShaderID, 1, &contentsChar, nullptr);
    glCompileShader(outShaderID);

    if (!IsCompiled(outShaderID)){
        SDL_Log("[SHADER] Failed to compile shader %s", shaderString.c_str());
        return false;
    }

    return true;
}

bool Shader::IsCompiled(const GLuint shader) {
    GLint status = 0;

    // Query the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[512] = {};
        glGetProgramInfoLog(shader, 512, nullptr, buffer);
        SDL_Log("[SHADER] GLSL Compile Failed:\n%s", buffer);
        return false;
    }
    return true;
}

bool Shader::IsValidProgram() const {
    GLint status = 0;

    // Query the link status
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[512] = {};
        glGetProgramInfoLog(mShaderProgram, 512, nullptr, buffer);
        SDL_Log("[SHADER] GLSL Link Status:\n%s", buffer);
        return false;
    }
    return true;
}