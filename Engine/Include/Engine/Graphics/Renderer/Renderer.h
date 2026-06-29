#pragma once
#include <vector>

#include "RenderUtils.h"
#include "Engine/Fwd/SDLFwd.h"
#include "Engine/Math/Math.h"

namespace RenderUtils {
    struct ModelCommand;
    struct DebugCommand;
}

class Drawer;
class FrameBuffer;
class PostEffectsManager;

class Renderer {
public:
    Renderer(SDL_Window* window, int width, int height);
    ~Renderer();

    void SetLightPosition(const Vector3& lightPosition);
    const Matrix4& GetLightViewProjectionMatrix();

    void SetViewMatrix(const Matrix4& viewMatrix);
    void SetProjectionMatrix(const Matrix4& projectionMatrix);
    const Matrix4& GetViewProjectionMatrix();

    void PushShadowCommand(RenderUtils::ShadowsCommand& cmd);
    void PushModelCommand(RenderUtils::ModelCommand& cmd);
    void PushSpritesCommand(RenderUtils::SpriteCommand& cmd);
    void PushDebugCommand(RenderUtils::DebugCommand& cmd);

    [[nodiscard]] Drawer* GetDrawer() const { return mDrawer.get(); }
    [[nodiscard]] PostEffectsManager* GetPostEffectsManager() const { return mPostEffectsManager.get(); }
private:
    [[nodiscard]] bool Init();
    void SetDebug(const bool enabled) { mIsDebugEnabled = enabled; }
    void SetLight(const bool enabled) { mIsLightEnabled = enabled; }

    // Drawing steps
    void Draw();

    SDL_Window* mEngineWindow;
    UniqueGLContext mContext;
    std::unique_ptr<Drawer> mDrawer;

    int mWindowWidth;
    int mWindowHeight;

    // Light matrices
    Vector3 mLightPosition;
    Matrix4 mLightViewMatrix{Matrix4::Identity};
    Matrix4 mLightProjectionMatrix{Matrix4::Identity};
    Matrix4 mLightViewProjectionMatrix{Matrix4::Identity};
    bool mIsLightMatrixDirty{true};
    float mLightOrthoSide;
    float mLightOrthoNear;
    float mLightOrthoFar;

    // Rendering matrices
    Matrix4 mOrthoMatrix;
    Matrix4 mViewMatrix{Matrix4::Identity};
    Matrix4 mProjectionMatrix{Matrix4::Identity};
    Matrix4 mViewProjectionMatrix{Matrix4::Identity};
    bool mIsViewProjectionDirty{true};

    // Commands queue
    std::vector<RenderUtils::ShadowsCommand> mShadowsCommands;
    std::vector<RenderUtils::ModelCommand> mModelCommands;
    std::vector<RenderUtils::SpriteCommand> mSpriteCommands;
    std::vector<RenderUtils::DebugCommand> mDebugCommands;

    // Frame Buffers
    std::unique_ptr<FrameBuffer> mPostEffectFrameBuffer{nullptr};
    std::unique_ptr<FrameBuffer> mShadowsFrameBuffer{nullptr};
    int mShadowMapSize{2048};

    // Post Effects
    std::unique_ptr<PostEffectsManager> mPostEffectsManager{nullptr};

    bool mIsDebugEnabled{false};
    bool mIsLightEnabled{true};

    friend class Engine;
};
