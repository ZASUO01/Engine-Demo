#include "Game.h"
#include <SDL2/SDL.h>
#include "Core/Actors/Ball.h"
#include "Core/Actors/Desk.h"
#include "Core/Cameras/MainCamera.h"
#include "Data/GameRecipe.h"
#include "Engine/Assets/AssetsManager.h"
#include "Engine/Config/ConfigManager.h"
#include "Engine/Contexts/Contexts.h"
#include "Engine/Core/Actors/ActorsManager.h"
#include "Engine/Core/Cameras/CamerasManager.h"
#include "Engine/Data/ResourcesLoader.h"
#include "Engine/Graphics/Model.h"
#include "Engine/Graphics/PostProcess/PostEffect.h"
#include "Engine/Graphics/PostProcess/PostEffectsManager.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Shader/ShadersManager.h"
#include "Engine/Math/Random.h"
#include "Engine/Sound/Synthesizer.h"
#include "Engine/UI/UIElementsManager.h"
#include "Engine/UI/Elements/UIText.h"
#include "Sound/Crystal.h"
#include <algorithm>

Game::~Game() = default;

bool Game::Init(EngineContext &ctx) {
    auto& camerasCtx = ctx.subsystems.camerasContext;
    auto& configCtx = ctx.resources.configContext;
    auto& loaderCtx = ctx.operations.loaderContext;

    configCtx.SetString("game.shaders.filter", "Source/Shaders/Filter");

    configCtx.SetString("game.assets.models.sphere", "Source/Assets/Models/Sphere");
    configCtx.SetString("game.assets.models.base", "Source/Assets/Models/Base");
    configCtx.SetString("game.assets.models.wall", "Source/Assets/Models/Wall");
    configCtx.SetString("game.assets.models.divider", "Source/Assets/Models/Divider");

    configCtx.SetString("game.assets.textures.sphere", "Source/Assets/Textures/Steel");
    configCtx.SetString("game.assets.textures.base", "Source/Assets/Textures/BaseColors");
    configCtx.SetString("game.assets.textures.alter", "Source/Assets/Textures/AlterColors");

    loaderCtx.SubmitLoadingInstruction(std::make_unique<GameRecipe>());

    auto width = configCtx.GetInt("engine.window.width");
    auto height = configCtx.GetInt("engine.window.height");

    camerasCtx.AddCamera(std::make_unique<MainCamera>(width, height));
    camerasCtx.SetCurrentCamera("MainCamera");

   return true;
}

void Game::OnResourcesReady(EngineContext &ctx) {
    const auto& configCtx = ctx.resources.configContext;
    const auto &renderCtx = ctx.operations.renderContext;
    auto &assetsCtx = ctx.resources.assetsContext;
    auto& actorsCtx = ctx.subsystems.actorsContext;
    auto& physicsCtx = ctx.subsystems.physicsContext;
    auto& shadersCtx = ctx.resources.shadersContext;
    auto& synthCTX = ctx.subsystems.synthContext;
    auto& UICtx = ctx.subsystems.uiElementsContext;

    mWindowWidth = configCtx.GetInt("engine.window.width");
    mHalfWindowWidth = mWindowWidth / 2;
    mWindowHeight = configCtx.GetInt("engine.window.height");
    mHalfWindowHeight = mWindowHeight / 2;

    // Filter
    const auto filterShaderName = configCtx.GetString("game.shaders.filter");
    if (const auto filterShader = shadersCtx.GetShader(filterShaderName)) {
        renderCtx.GetPostEffectsManager()->AddEffect(std::make_unique<PostEffect>(filterShader));
    }

    // Ball
    const auto ballModel = assetsCtx.AddModel("Ball");

    const auto ballMeshName = configCtx.GetString("game.assets.models.sphere");
    if (const auto ballMesh = assetsCtx.GetMesh(ballMeshName)) {
        ballModel->SetMesh(ballMesh);
    }

    const auto ballTexName = configCtx.GetString("game.assets.textures.sphere");
    if (const auto ballTex = assetsCtx.GetTexture(ballTexName)) {
        ballModel->AddTexture(ballTex);
    }

    auto ball = std::make_unique<Ball>(ballModel, &physicsCtx);
    ball->SetPosition(Vector3{0.0f, 3.0f, 0.0f});
    actorsCtx.AddActor(std::move(ball));

    // Base
    const auto baseModel = assetsCtx.AddModel("Base");

    const auto baseMeshName = configCtx.GetString("game.assets.models.base");
    if (const auto baseMesh = assetsCtx.GetMesh(baseMeshName)) {
        baseModel->SetMesh(baseMesh);
    }

    const auto baseTexName = configCtx.GetString("game.assets.textures.base");
    const auto baseTex = assetsCtx.GetTexture(baseTexName);
    if (baseTex) {
        baseModel->AddTexture(baseTex);
    }

    // Wall
    const auto wallModel = assetsCtx.AddModel("Wall");

    const auto WallMeshName = configCtx.GetString("game.assets.models.wall");
    if (const auto wallMesh = assetsCtx.GetMesh(WallMeshName)) {
        wallModel->SetMesh(wallMesh);
    }

    if (baseTex) {
        wallModel->AddTexture(baseTex);
    }

    // Divider
    const auto dividerModel = assetsCtx.AddModel("Divider");

    const auto dividerMeshName = configCtx.GetString("game.assets.models.divider");
    if (const auto dividerMesh = assetsCtx.GetMesh(dividerMeshName)) {
        dividerModel->SetMesh(dividerMesh);
    }

    const auto dividerTexName = configCtx.GetString("game.assets.textures.alter");
    if (const auto dividerTex = assetsCtx.GetTexture(dividerTexName)) {
        dividerModel->AddTexture(dividerTex);
    }

    Desk::DeskModels dkModels = { baseModel, wallModel, dividerModel };
    auto actor = std::make_unique<Desk>(dkModels, &actorsCtx, &physicsCtx);
    mDesk = actor.get();

    actorsCtx.AddActor(std::move(actor));

    // TEXTS
    LoadTexts(ctx);

    // SYNTH
    synthCTX.SetTimbre(Timbre::CreateUnique<Crystal>());
    mRandomNotes.reserve(4);

    mGameState = GameState::Initial;
    SetInitialState();
}

void Game::Update(float deltaTime, EngineContext &ctx) {
    switch (mGameState) {
        case GameState::Loading:
            break;
        case GameState::Initial:
            break;
        case GameState::Playing:
            UpdatePlayingState(deltaTime, ctx);
            break;
        default:
            break;
    }
}

void Game::ProcessInput(const int key) {
    if (mGameState == GameState::Initial) {
        if (key == SDL_SCANCODE_RETURN) {
            mGameState = GameState::Playing;
            SetPlayingWaiting();
        }
    }else if (mGameState == GameState::Playing) {
        if (key == SDL_SCANCODE_ESCAPE) {
            mGameState = GameState::Initial;
            SetInitialState();
        }
    }
}

void Game::LoadTexts(const EngineContext& ctx) {
    const auto& configCtx = ctx.resources.configContext;
    auto& assetsCtx = ctx.resources.assetsContext;
    auto& uiCtx = ctx.subsystems.uiElementsContext;

    const auto fontName = configCtx.GetString("engine.assets.fonts.mono");
    if (const auto font = assetsCtx.GetFont(fontName)) {
        auto interactionText = std::make_unique<UIText>();
        interactionText->SetFont(font);

        auto countDownText = std::make_unique<UIText>();
        countDownText->SetFont(font);

        auto scoreText = std::make_unique<UIText>();
        scoreText->SetFont(font);

        mInterationText = uiCtx.AddText(std::move(interactionText));
        mCounterText = uiCtx.AddText(std::move(countDownText));
        mScoreText = uiCtx.AddText(std::move(scoreText));
    }
}

void Game::SetInitialState() const {
    // INTERACTION TEXT
    mInterationText->SetFontSize(36);
    mInterationText->SetColor(Vector3{0.6f, 0.6f, 0.6f});
    mInterationText->SetText("Press enter to start");
    mInterationText->SetOffset(Vector2{mHalfWindowWidth - 210, mHalfWindowHeight - 200});

    // Countdown TEXT
    mCounterText->SetVisible(false);
    mScoreText->SetVisible(false);
}

void Game::UpdatePlayingState(const float deltaTime, const EngineContext &ctx) {
    auto& synthCtx = ctx.subsystems.synthContext;

    switch (mPlayingState) {
        case PlayingState::Waiting:
            mWaitingDuration -= deltaTime;
            if (mWaitingDuration <= 0) {
                mSecondsRemaining--;

                if (mSecondsRemaining <= 0) {
                    GenerateRandomNotes(mRandomNotes);
                    SetPlayingListening();
                    mPlayingState = PlayingState::Listening;
                }else {
                    mCounterText->SetText(std::to_string(mSecondsRemaining));
                }

                mWaitingDuration = 1.0f;
            }
            break;
        case PlayingState::Listening:
            mListenDuration -= deltaTime;
            if (mListenDuration <= 0) {
                if (mNotesRemaining >= 4) {
                    SetPlayingChoosing();
                    mPlayingState = PlayingState::Trial;
                }else {
                    const auto note = mRandomNotes[mNotesRemaining];
                    synthCtx.StopNote();
                    synthCtx.PlayNote(note, 3, 1.0f);

                    mNotesRemaining++;
                }

                mListenDuration = 1.2f;
            }
            break;
        case PlayingState::Trial:
            mWaitingDuration -= deltaTime;
            if (mWaitingDuration <= 0) {
                mSecondsRemaining--;

                if (mSecondsRemaining <= 0) {
                    SetPlayingChecking();
                    mPlayingState = PlayingState::Check;
                }else {
                    mCounterText->SetText(std::to_string(mSecondsRemaining));

                    if (const auto chosenOption = mDesk->GetCurrentOption(); mLastOption != chosenOption) {
                        const auto note = mRandomNotes[chosenOption];

                        synthCtx.StopNote();
                        synthCtx.PlayNote(note, 3, 1.0f);
                        mLastOption = chosenOption;
                    }
                }
                mWaitingDuration = 1.0f;
            }
            break;
        case PlayingState::Check:
                mWaitingDuration -= deltaTime;
                if (mWaitingDuration <= 0) {
                    SetPlayingWaiting();
                    mPlayingState = PlayingState::Waiting;
                }
                break;
        default:
            break;
    }
}

void Game::GenerateRandomNotes(std::vector<SynthUtils::Note> &notes) {
    notes.clear();

    constexpr int maxIndex = static_cast<int>(SynthUtils::Note::Count) - 1;

    while (notes.size() < 4) {
        const int randomVal = Random::GetIntRange(0, maxIndex);

        if (const auto newNote = static_cast<SynthUtils::Note>(randomVal); std::ranges::find(notes, newNote) == notes.end()) {
            notes.push_back(newNote);
        }
    }
}

void Game::SetPlayingWaiting() {
    mSecondsRemaining = 3;
    mWaitingDuration = 1.0f;
    mLastOption = -1;
    mDesk->ResetOption();

    mInterationText->SetText("Prepare yourself!");
    mInterationText->SetOffset(Vector2{mHalfWindowWidth - 180, mHalfWindowHeight - 200});

    mCounterText->SetVisible(true);
    mCounterText->SetOffset(Vector2{mHalfWindowWidth - 10, mHalfWindowHeight - 150});
    mCounterText->SetText(std::to_string(mSecondsRemaining));

    mScoreText->SetVisible(true);
    mScoreText->SetColor(Color::White);
    mScoreText->SetOffset(Vector2{mHalfWindowWidth - 90, 40});
    mScoreText->SetText("Score: "+std::to_string(mScore));
}

void Game::SetPlayingListening() {
    mListenDuration = 1.2f;
    mNotesRemaining = 0;

    mInterationText->SetText("Listen carefully");
    mInterationText->SetOffset(Vector2{mHalfWindowWidth - 160, mHalfWindowHeight - 200});

    mCounterText->SetVisible(false);
}

void Game::SetPlayingChoosing()  {
    mSecondsRemaining = 10;
    mWaitingDuration = 1.0f;
    mLastOption = -1;

    const int randomVal = Random::GetIntRange(0, 3);
    mGameOption = randomVal;

    mInterationText->SetText("Choose the sound number " + std::to_string(randomVal + 1));
    mInterationText->SetOffset(Vector2{mHalfWindowWidth - 240, mHalfWindowHeight - 200});

    mCounterText->SetVisible(true);
}

void Game::SetPlayingChecking() {
    mWaitingDuration = 2.0f;
    mInterationText->SetOffset(Vector2{mHalfWindowWidth - 60, mHalfWindowHeight - 200});
    if (mGameOption == mLastOption) {
        mInterationText->SetText("Correct");
        mScore++;
    }else {
        mInterationText->SetText("Wrong");
    }

    mCounterText->SetVisible(false);
}
