#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Utils/SDLUtils.h"
#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "Engine/Graphics/FrameBuffer.h"
#include "Engine/Graphics/PostProcess/PostEffectsManager.h"
#include "Engine/Graphics/Renderer/Drawer.h"
#include "Engine/Graphics/Renderer/RenderUtils.h"
#include "Engine/Graphics/Resources/Texture.h"

Renderer::Renderer(SDL_Window* window, const int width, const int height)
:mEngineWindow(window), mWindowWidth(width), mWindowHeight(height) {

    mOrthoMatrix = Matrix4::CreateOrtho(0, static_cast<float>(mWindowWidth), static_cast<float>(mWindowHeight), 0, -1.0, 1.0);

    mLightPosition = Vector3{15.0f, 30.0f, 15.0f};
    mLightOrthoNear = 1.0f;
    mLightOrthoFar = 100.f;
    mLightOrthoSide = 12.0f;
    mLightProjectionMatrix = Matrix4::CreateOrtho(-mLightOrthoSide, mLightOrthoSide, -mLightOrthoSide, mLightOrthoSide, mLightOrthoNear, mLightOrthoFar);
}

Renderer::~Renderer() = default;

bool Renderer::Init() {
    mContext = SDLUtils::CreateGLContext(mEngineWindow);
    if (!mContext) {
        return false;
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        SDL_Log("[RENDERER] Failed to init GLAD.");
        return false;
    }

    // Set Vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
        SDL_Log("[RENDERER] Failed to set VSync: %s", SDL_GetError());
        return false;
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    mDrawer = std::make_unique<Drawer>();
    if (!mDrawer->Init()) {
        return false;
    }

    FrameBufferSpecification shadowSpec = {
        mShadowMapSize,
        mShadowMapSize,
        FrameBufferFormat::DepthOnlyTexture
    };
    mShadowsFrameBuffer = std::make_unique<FrameBuffer>(shadowSpec);

    FrameBufferSpecification postEffectSpec = {
        mWindowWidth,
        mWindowHeight,
        FrameBufferFormat::ColorWithDepthRBO
    };
    mPostEffectFrameBuffer = std::make_unique<FrameBuffer>(postEffectSpec);

    mPostEffectsManager = std::make_unique<PostEffectsManager>();

    return true;
}

void Renderer::Draw() {
    if (mIsLightEnabled) {
        mShadowsFrameBuffer->Bind();
        glViewport(0, 0, mShadowMapSize, mShadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);

        // SHADOW PASS
        mDrawer->DrawShadowPass(mShadowsCommands, GetLightViewProjectionMatrix());

        FrameBuffer::Unbind();
    }

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glViewport(0, 0, mWindowWidth, mWindowHeight);

    // Set post Effects
    if (mPostEffectsManager->HasActiveEffect()) {
        mPostEffectFrameBuffer->Bind();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    constexpr int BUFFER_TEXTURE_SLOT = 1;
    if (mIsLightEnabled) {
        mShadowsFrameBuffer->GetTexture()->SetActive(BUFFER_TEXTURE_SLOT);
        glActiveTexture(GL_TEXTURE0);
    }

    Vector3 lightDir =  mLightPosition;
    lightDir.Normalize();

    // MODELS PASS
    if (mIsLightEnabled) {
        const Drawer::ModelPassData data = { GetViewProjectionMatrix(), GetLightViewProjectionMatrix(), lightDir, BUFFER_TEXTURE_SLOT};
        mDrawer->DrawModelPass(mModelCommands, true, data);
    }else {
        const Drawer::ModelPassData data { GetViewProjectionMatrix()};
        mDrawer->DrawModelPass(mModelCommands, false, data);
    }

    // SPRITES PASS
    mDrawer->DrawSpritesPass(mSpriteCommands, mOrthoMatrix);

    // POST PASS
    if (mPostEffectsManager->HasActiveEffect()) {
        FrameBuffer::Unbind();
        mPostEffectFrameBuffer->GetTexture()->SetActive(BUFFER_TEXTURE_SLOT);
        glActiveTexture(GL_TEXTURE0);

        const auto effect = mPostEffectsManager->GetCurrentEffect();
        mDrawer->DrawPostPass(effect, BUFFER_TEXTURE_SLOT);
    }

    // DEBUG PASS
    if (mIsDebugEnabled) {
        mDrawer->DrawDebugPass(mDebugCommands, GetViewProjectionMatrix());
    }

    SDL_GL_SwapWindow(mEngineWindow);
}


void Renderer::SetLightPosition(const Vector3 &lightPosition) {
    if (!mIsLightEnabled) {
        return;
    }

    mLightPosition = lightPosition;
    mIsLightMatrixDirty = true;
}

const Matrix4 &Renderer::GetLightViewProjectionMatrix() {
    if (mIsLightMatrixDirty) {
        mIsLightMatrixDirty = false;

        mLightViewMatrix = Matrix4::CreateLookAt(mLightPosition, Vector3{0.0f, 0.0f, 0.0f}, Vector3::UnitY);
        mLightViewProjectionMatrix = mLightViewMatrix * mLightProjectionMatrix;
    }

    return mLightViewProjectionMatrix;
}

void Renderer::SetViewMatrix(const Matrix4 &viewMatrix) {
    mViewMatrix = viewMatrix;
    mIsViewProjectionDirty = true;
}

void Renderer::SetProjectionMatrix(const Matrix4 &projectionMatrix) {
    mProjectionMatrix = projectionMatrix;
    mIsViewProjectionDirty = true;
}

const Matrix4& Renderer::GetViewProjectionMatrix() {
    if (mIsViewProjectionDirty) {
        mIsViewProjectionDirty = false;
        mViewProjectionMatrix = mViewMatrix * mProjectionMatrix;
    }

    return mViewProjectionMatrix;
}

void Renderer::PushShadowCommand(RenderUtils::ShadowsCommand &cmd) {
    mShadowsCommands.emplace_back(cmd);
}

void Renderer::PushModelCommand(RenderUtils::ModelCommand &cmd) {
    mModelCommands.emplace_back(cmd);
}

void Renderer::PushSpritesCommand(RenderUtils::SpriteCommand &cmd) {
    mSpriteCommands.emplace_back(cmd);
}

void Renderer::PushDebugCommand(RenderUtils::DebugCommand &cmd) {
    mDebugCommands.emplace_back(cmd);
}
