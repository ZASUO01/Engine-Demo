#include "Engine/Graphics/Model.h"
#include "Engine/Graphics/Geometry/Mesh.h"
#include "Engine/Graphics/Resources/Texture.h"
#include "Engine/Graphics/Shader/Shader.h"

Model::Model(Mesh* baseMesh, Texture* baseTexture)
:mShader(nullptr)
,mMesh(baseMesh)
{
    AddTexture(baseTexture);
}

Model::~Model() = default;

unsigned int Model::GetShaderID() const {
    return mShader->GetID();
}

VertexArray* Model::GetVertexArray() const {
    return mMesh->GetVertexArray();
}

unsigned int Model::GetVertexArrayID() const {
    return mMesh->GetVertexArrayID();
}

Texture *Model::GetTexture(const int index) const {
    if (mTextures.empty() || index < 0 || index >= mTextures.size()) {
        return nullptr;
    }

    return mTextures[index];
}

unsigned int Model::GetTextureID(const int index) const {
    if (mTextures.empty() || index < 0 || index >= mTextures.size()) {
        return 0;
    }

    return mTextures[index]->GetID();
}
