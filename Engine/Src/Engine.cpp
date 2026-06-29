#include "Engine/Engine.h"
#include <SDL2/SDL.h>
#include "Engine/Assets/AssetsManager.h"
#include "Engine/Config/ConfigManager.h"
#include "Engine/Config/ConfigFlags.h"
#include "Engine/Contexts/Contexts.h"
#include "Engine/Core/GlobalLight.h"
#include "Engine/Data/EngineRecipe.h"
#include "Engine/Data/ResourcesLoader.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Shader/ShadersManager.h"
#include "Engine/Math/Random.h"
#include "Engine/Utils/SDLUtils.h"
#include "Engine/Core/UserApplication.h"
#include "Engine/Core/Actors/ActorsManager.h"
#include "Engine/Core/Cameras/CamerasManager.h"
#include "Engine/Core/Cameras/DefaultCamera.h"
#include "Engine/Graphics/PostProcess/PostEffectsManager.h"
#include "Engine/Graphics/Renderer/Drawer.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Sound/Synthesizer.h"
#include "Engine/UI/UIElementsManager.h"

Engine::Engine(std::unique_ptr<UserApplication> app) {
    mApp = std::move(app);
}

Engine::~Engine() {
    ShutDown();
}

bool Engine::Initialize() {
    if (mState != State::DOWN) {
        return false;
    }
    mState = State::STARTING;

    if (!SDLUtils::InitSDLSystems()) {
        return false;
    }

    if (!Setup()) {
        return false;
    }

    ResourcesContext resCtx { *mConfigManager, *mShadersManager, *mAssetsManager };
    OperationsContext opCtx { *mResourcesLoader, *mRenderer };
    SubsystemsContext subCtx { *mActorsManager, *mPhysicsSystem, *mCamerasManager, *mSynthesizer, *mUIElementsManager};

    if (EngineContext ctx { resCtx, opCtx, subCtx }; !mApp->Init(ctx)) {
        return false;
    }

    mState = State::READY;
    return true;
}

void Engine::RunLoop() {
    if (mState != State::READY) {
        return;
    }

    mIsRunning = true;
    mTicksCount = SDL_GetTicks();

    ResourcesContext resCtx { *mConfigManager, *mShadersManager, *mAssetsManager };
    OperationsContext opCtx { *mResourcesLoader, *mRenderer };
    SubsystemsContext subCtx { *mActorsManager, *mPhysicsSystem, *mCamerasManager, *mSynthesizer, *mUIElementsManager };
    EngineContext ctx { resCtx, opCtx, subCtx };

    while (mIsRunning) {
        const uint32_t frameStartTicks = SDL_GetTicks();
        float deltaTime = static_cast<float>(frameStartTicks - mTicksCount) / 1000.0f;

        if (deltaTime > 0.05f){
            deltaTime = 0.05f;
        }
        mTicksCount = frameStartTicks;

        switch (mResourcesLoader->GetState()) {
            case ResourcesLoader::State::Empty:
                SDL_Log("Empty...");
                break;
            case ResourcesLoader::State::Loading:
                SDL_Log("Loading...");
                break;
            case ResourcesLoader::State::Loaded:
                if (mResourcesLoader->ConcludeLoadingProcess()) {
                    mApp->OnResourcesReady(ctx);
                }
                break;
            case ResourcesLoader::State::Ready:
                ProcessInput();
                UpdateEngine(deltaTime, ctx);
                break;
            default:
                break;
        }

        GenerateOutput();

        const Uint32 frameElapsedTicks = SDL_GetTicks() - frameStartTicks;

        if (constexpr Uint32 targetMs = 1000 / 60; frameElapsedTicks < targetMs) {
            const Uint32 sleepTime = targetMs - frameElapsedTicks;
            SDL_Delay(sleepTime);
        }
    }
}

bool Engine::Setup() {
    Random::Init();

    mConfigManager = std::make_unique<ConfigManager>();
    const auto title = mConfigManager->GetString("engine.window.title");
    const auto width = mConfigManager->GetInt("engine.window.width");
    const auto height = mConfigManager->GetInt("engine.window.height");
    const auto shadowShaderName = mConfigManager->GetString("engine.shaders.shadow");
    const auto modelShaderName = mConfigManager->GetString("engine.shaders.models");
    const auto shadowModelsShaderName = mConfigManager->GetString("engine.shaders.shadowModels");
    const auto uiShaderName = mConfigManager->GetString("engine.shaders.ui");
    const auto debugShaderName = mConfigManager->GetString("engine.shaders.debug");
    mFlags = static_cast<DrawMode>(mConfigManager->GetUnsigned("engine.draw.mask"));

    mWindow = SDLUtils::CreateSDLWindow(title.c_str(), width, height);
    if (!mWindow) {
        return false;
    }

    mRenderer = std::make_unique<Renderer>(mWindow.get(), width, height);
    if (!mRenderer->Init()) {
        return false;
    }

    mRenderer->SetDebug(HasFlag(mFlags, DrawMode::Collision));
    mRenderer->SetLight(HasFlag(mFlags, DrawMode::Shadow));

    // Shaders Manager
    mShadersManager = std::make_unique<ShadersManager>();
    if (!shadowShaderName.empty()) {
        mShadersManager->ListenForShader(shadowShaderName, [this](Shader* shader) {
           mRenderer->GetDrawer()->SetShadowShader(shader);
        });
    }

    if (!modelShaderName.empty()) {
        mShadersManager->ListenForShader(modelShaderName, [this](Shader* shader) {
            mRenderer->GetDrawer()->SetModelsShader(shader);
        });
    }

    if (!shadowModelsShaderName.empty()) {
        mShadersManager->ListenForShader(shadowModelsShaderName, [this](Shader* shader) {
           mRenderer->GetDrawer()->SetShadowModelsShader(shader);
        });
    }

    if (!uiShaderName.empty()) {
        mShadersManager->ListenForShader(uiShaderName, [this](Shader* shader) {
            mRenderer->GetDrawer()->SetUIShader(shader);
        });
    }

    if (!debugShaderName.empty()) {
        mShadersManager->ListenForShader(debugShaderName, [this](Shader* shader) {
            mRenderer->GetDrawer()->SetDebugShader(shader);
        });
    }

    mAssetsManager = std::make_unique<AssetsManager>();

    ResourcesContext ctx {*mConfigManager, *mShadersManager, *mAssetsManager} ;

    mResourcesLoader = std::make_unique<ResourcesLoader>(ctx);
    mResourcesLoader->SubmitLoadingInstruction(std::make_unique<EngineRecipe>());

    mCamerasManager = std::make_unique<CamerasManager>();
    mCamerasManager->AddCamera(std::make_unique<DefaultCamera>(1024, 768));
    mCamerasManager->SetCurrentCamera("Default");

    mActorsManager = std::make_unique<ActorsManager>();
    mActorsManager->SetDrawMode(mFlags);
    mPhysicsSystem = std::make_unique<PhysicsSystem>();

    mGlobalLight = std::make_unique<GlobalLight>(15.0f, 30.0f);

    mSynthesizer = std::make_unique<Synthesizer>();
    if (!mSynthesizer->Init()) {
        return false;
    }

    mUIElementsManager = std::make_unique<UIElementsManager>();

    return true;
}

void Engine::ShutDown() {
    if (mState == State::DOWN) {
        return;
    }

    mActorsManager.reset();
    mCamerasManager.reset();
    mResourcesLoader.reset();
    mAssetsManager.reset();
    mShadersManager.reset();
    mConfigManager.reset();
    mRenderer.reset();
    mWindow.reset();
    SDLUtils::ShutdownSDLSystems();
}

void Engine::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                ProcessKeydownEvents(event.key.keysym.scancode);
                break;
            default:
                break;
        }
    }

    const Uint8 *keyState = SDL_GetKeyboardState(nullptr);
    ProcessKeyState(keyState);
}

void Engine::UpdateEngine(const float deltaTime, EngineContext &ctx) const {
    mCamerasManager->UpdateCurrentCamera(deltaTime);
    mPhysicsSystem->Update();
    mActorsManager->UpdateActors(deltaTime, mRenderer.get());
    mGlobalLight->Update(deltaTime);
    mSynthesizer->Update();
    mUIElementsManager->SendDrawCommands(mRenderer.get());

    mApp->Update(deltaTime, ctx);
}

void Engine::GenerateOutput() const {
    mCamerasManager->SetRendererMatrices(mRenderer.get());
    mGlobalLight->SetRenderLight(mRenderer.get());

    mRenderer->Draw();
}

void Engine::ProcessKeydownEvents(const int key) {
    mCamerasManager->ProcessCurrentCameraInput(key);
    mGlobalLight->ProcessInput(key);

    switch (key) {
        case SDL_SCANCODE_1:
            ToggleModels();
            break;
        case SDL_SCANCODE_2:
            ToggleLight();
            break;
        case SDL_SCANCODE_3:
            TogglePostProcess();
            break;
        case SDL_SCANCODE_4:
            ToggleCollision();
            break;
        default:
            break;
    }

    mApp->ProcessInput(key);
}

void Engine::ProcessKeyState(const Uint8 *state) const {
    mActorsManager->SendInputToActors(state);
}

void Engine::ToggleLight()  {
    ToggleFlag(mFlags, DrawMode::Shadow);
    const bool lightEnabled = HasFlag(mFlags, DrawMode::Shadow);

    mActorsManager->SetDrawMode(mFlags);
    mGlobalLight->SetLightEnabled(lightEnabled);
    mRenderer->SetLight(lightEnabled);
}

void Engine::ToggleCollision() {
    ToggleFlag(mFlags, DrawMode::Collision);
    const bool collisionEnabled = HasFlag(mFlags, DrawMode::Collision);

    mRenderer->SetDebug(collisionEnabled);
    mActorsManager->SetDrawMode(mFlags);
}

void Engine::ToggleModels() {
    ToggleFlag(mFlags, DrawMode::Models);

    mActorsManager->SetDrawMode(mFlags);

}

void Engine::TogglePostProcess() {
    ToggleFlag(mFlags, DrawMode::PostProcess);

    if (HasFlag(mFlags, DrawMode::PostProcess)) {
        mRenderer->GetPostEffectsManager()->SetCurrentEffect(0);
        return;
    }

    mRenderer->GetPostEffectsManager()->DisableEffect();
}
