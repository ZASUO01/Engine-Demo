#pragma once
#include "Engine/Fwd/SDLFwd.h"
#include "Engine/Fwd/EngineFwd.h"

class Engine {
public:
    enum class State {
        DOWN,
        STARTING,
        READY,
    };

    explicit Engine(std::unique_ptr<UserApplication> app);
    ~Engine();

    [[nodiscard]] bool Initialize();
    void Quit() { mIsRunning = false; }
    void RunLoop();

private:
    [[nodiscard]] bool Setup();
    void ShutDown();
    void ProcessInput();
    void UpdateEngine(float deltaTime, EngineContext &ctx) const;
    void GenerateOutput() const;

    void ProcessKeydownEvents(int key);
    void ProcessKeyState(const Uint8* state) const;

    void ToggleLight();
    void ToggleModels();
    void ToggleCollision();
    void TogglePostProcess();

    UniqueWindow mWindow{nullptr};

    // Application
    UniqueUserApp mApp{nullptr};

    // Resources
    UniqueConfigManager mConfigManager{nullptr};
    UniqueShadersManager mShadersManager{nullptr};
    UniqueAssetsManager mAssetsManager{nullptr};

    // Operations
    UniqueRenderer mRenderer{nullptr};
    UniqueResourcesLoader mResourcesLoader{nullptr};

    // Subsystems
    UniqueCamerasManager mCamerasManager{nullptr};
    UniqueActorsManager mActorsManager{nullptr};
    UniquePhysicsSystem mPhysicsSystem{nullptr};
    UniqueSynthesizer mSynthesizer{nullptr};
    UniqueUIElementsManager mUIElementsManager{nullptr};

    // Misc
    UniqueGlobalLight mGlobalLight{nullptr};

    // Flags control
    DrawMode mFlags{0};

    State mState{State::DOWN};
    bool mIsRunning{false};
    uint32_t mTicksCount{0};
};
