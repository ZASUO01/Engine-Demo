#pragma once
#include <vector>
#include "Core/Actors/Desk.h"
#include "Engine/Core/UserApplication.h"
#include "Engine/Sound/SynthUtils.h"

class UIText;

class Game : public UserApplication {
public:
    ~Game() override;

    [[nodiscard]] bool Init(EngineContext &ctx) override;
    void OnResourcesReady(EngineContext &ctx) override;
    void Update(float deltaTime, EngineContext &ctx) override;
    void ProcessInput(int key) override;

private:
    enum class GameState {
        Loading,
        Initial,
        Playing,
    };

    enum class PlayingState {
        Waiting,
        Listening,
        Trial,
        Check
    };

    void LoadTexts(const EngineContext& ctx);

    void SetInitialState() const;

    void UpdatePlayingState(float deltaTime, const EngineContext &ctx);

    static void GenerateRandomNotes(std::vector<SynthUtils::Note>& notes);

    // usefull variables
    int mWindowWidth{0};
    int mHalfWindowWidth{0};
    int mWindowHeight{0};
    int mHalfWindowHeight{0};

    // Playing variables
    // Waiting
    float mWaitingDuration{1.0f};
    int mSecondsRemaining{5};
    void SetPlayingWaiting();

    // Listening
    std::vector<SynthUtils::Note> mRandomNotes;
    float mListenDuration{1.2f};
    int mNotesRemaining{4};
    void SetPlayingListening();

    // Choosing
    int mGameOption{-1};
    void SetPlayingChoosing();
    int mLastOption{-1};

    int mScore{0};

    void SetPlayingChecking();

    Desk* mDesk{nullptr};

    UIText* mInterationText{nullptr};
    UIText* mCounterText{nullptr};
    UIText* mScoreText{nullptr};

    GameState mGameState{GameState::Loading};
    PlayingState mPlayingState{PlayingState::Waiting};
};
