// Flappy Bird feito com a Engine: todo o jogo cabe neste arquivo.
// Fisica (gravidade, triggers), camera, render, texto e loop vem da Engine.
#include <SDL2/SDL.h>
#include <functional>
#include <string>
#include <vector>
#include "Engine/Engine.h"
#include "Engine/Assets/AssetsManager.h"
#include "Engine/Contexts/Contexts.h"
#include "Engine/Core/UserApplication.h"
#include "Engine/Core/Actors/Actor.h"
#include "Engine/Core/Actors/ActorsManager.h"
#include "Engine/Core/Cameras/Camera.h"
#include "Engine/Core/Cameras/CamerasManager.h"
#include "Engine/Core/Components/Drawing/ModelComponent.h"
#include "Engine/Core/Components/Physics/BoxColliderComponent.h"
#include "Engine/Core/Components/Physics/RigidBodyComponent.h"
#include "Engine/Core/Components/Physics/SphereColliderComponent.h"
#include "Engine/Graphics/Model.h"
#include "Engine/Graphics/Renderer/Renderer.h"
#include "Engine/Graphics/Resources/Texture.h"
#include "Engine/Math/Random.h"
#include "Engine/UI/UIElementsManager.h"
#include "Engine/UI/Elements/UIText.h"

constexpr float kWorldTop = 4.5f, kGap = 2.8f, kSpeed = 3.5f, kSpawnEvery = 1.7f, kFlap = 6.0f;

struct World { bool running{false}; int round{0}; };

// Textura de cor solida criada em memoria: o jogo nao depende de nenhum arquivo de imagem.
Texture* SolidColor(AssetsManager& assets, const std::string& name, Uint8 r, Uint8 g, Uint8 b) {
    const UniqueSurface surface(SDL_CreateRGBSurfaceWithFormat(0, 4, 4, 32, SDL_PIXELFORMAT_RGBA32));
    SDL_FillRect(surface.get(), nullptr, SDL_MapRGBA(surface->format, r, g, b, 255));
    auto* texture = assets.AddTexture(name);
    return texture->BuildFromSurface(surface) ? texture : nullptr;
}

class SideCamera : public Camera {
public:
    SideCamera(float w, float h) : Camera("Side") {
        mEye = Vector3{0.0f, 0.0f, -9.0f}; mWidth = w; mHeight = h;
        Set3DView(); Set3DProjection();
    }
};

// Um cubo da Engine, escalado e com textura.
class Block : public Actor {
public:
    Block(Model* model, const Vector3& pos, const Vector3& halfSize) {
        auto mesh = std::make_unique<ModelComponent>(this, model);
        mesh->SetTextureIndex(1);
        AddComponent(std::move(mesh));
        SetPosition(pos); SetScale(halfSize);
    }
};

// Ator invisivel so com um trigger de caixa (sem escala, para o broad phase da Engine medir certo).
class HitBox : public Actor {
public:
    HitBox(PhysicsSystem* physics, const Vector3& pos, const Vector3& halfSize, const std::function<void()>& onHit) {
        auto box = std::make_unique<BoxColliderComponent>(this, physics, halfSize, true);
        box->SetCollisionCallBack(onHit);
        AddComponent(std::move(box));
        SetPosition(pos);
    }
};

class Bird : public Block {
public:
    Bird(Model* model, PhysicsSystem* physics) : Block(model, Vector3{-2.0f, 0.0f, 0.0f}, Vector3{0.3f, 0.3f, 0.3f}) {
        auto body = std::make_unique<RigidBodyComponent>(this);
        body->SetGravity(16.0f);
        mBody = body.get();
        AddComponent(std::make_unique<SphereColliderComponent>(this, physics, 0.3f));
        AddComponent(std::move(body));
    }
    void Flap() const { mBody->SetVelocity(Vector3{0.0f, kFlap, 0.0f}); }
    void Freeze(bool frozen) const { mBody->SetApplyGravity(!frozen); mBody->SetVelocity(Vector3::Zero); }

protected:
    void OnUpdate(float) override { SetRotation(Vector3{0.0f, 0.0f, Math::Clamp(mBody->GetVelocity().y * 0.1f, -0.8f, 0.5f)}); }

private:
    RigidBodyComponent* mBody;
};

// Par de canos: o proprio ator e o trigger de ponto (o vao); os filhos sao os canos e suas hitboxes.
class Pipes : public Actor {
public:
    Pipes(Model* model, PhysicsSystem* physics, ActorsManager& actors, const World& world,
          const std::function<void()>& onHit, const std::function<void()>& onScore)
        : mWorld(world), mRound(world.round) {
        SetPosition(Vector3{9.0f, Random::GetFloatRange(-2.0f, 2.0f), 0.0f});

        auto gate = std::make_unique<BoxColliderComponent>(this, physics, Vector3{0.1f, kGap / 2, 1.0f}, true);
        gate->SetCollisionCallBack([this, onScore] { if (!mScored) { mScored = true; onScore(); } });
        AddComponent(std::move(gate));

        for (float side : {1.0f, -1.0f}) {
            const Vector3 pos{0.0f, side * (kGap / 2 + 5.0f), 0.0f}, halfSize{0.6f, 5.0f, 0.6f};
            Attach(actors, std::make_unique<Block>(model, pos, halfSize));
            Attach(actors, std::make_unique<HitBox>(physics, pos, halfSize, onHit));
        }
    }

protected:
    void OnUpdate(float dt) override {
        if (mWorld.running) SetPosition(mPosition - Vector3{kSpeed * dt, 0.0f, 0.0f});
        if (mPosition.x < -10.0f || mWorld.round != mRound) {
            SetState(State::Destroy);
            for (auto* part : mParts) part->SetState(State::Destroy);
        }
    }

private:
    void Attach(ActorsManager& actors, std::unique_ptr<Actor> part) {
        part->SetParent(this);
        mParts.push_back(actors.AddActor(std::move(part)));
    }

    const World& mWorld;
    int mRound;
    bool mScored{false};
    std::vector<Actor*> mParts;
};

class FlappyBird : public UserApplication {
public:
    bool Init(EngineContext& ctx) override {
        ctx.subsystems.camerasContext.AddCamera(std::make_unique<SideCamera>(1024.0f, 768.0f));
        ctx.subsystems.camerasContext.SetCurrentCamera("Side");
        return true;
    }

    void OnResourcesReady(EngineContext& ctx) override {
        auto& assets = ctx.resources.assetsContext;
        auto model = [&](const std::string& name, Uint8 r, Uint8 g, Uint8 b) {
            auto* m = assets.AddModel(name);
            m->AddTexture(SolidColor(assets, name, r, g, b));
            return m;
        };
        mActors = &ctx.subsystems.actorsContext;
        mPhysics = &ctx.subsystems.physicsContext;
        mPipeModel = model("Pipe", 80, 185, 60);
        ctx.operations.renderContext.SetLightPosition(Vector3{-6.0f, 20.0f, -15.0f}); // luz vindo da frente

        mBird = static_cast<Bird*>(mActors->AddActor(std::make_unique<Bird>(model("Bird", 255, 205, 40), mPhysics)));
        mActors->AddActor(std::make_unique<Block>(model("Ground", 205, 165, 95), Vector3{0.0f, -kWorldTop - 0.5f, 0.0f}, Vector3{12.0f, 0.5f, 1.0f}));

        auto* font = assets.GetFont("Engine/Assets/Fonts/JetBrainsMono");
        mScore = AddText(ctx, font, 64, Vector2{490.0f, 30.0f});
        mMessage = AddText(ctx, font, 36, Vector2{320.0f, 300.0f});
        Reset();
    }

    void ProcessInput(int key) override {
        if (key != SDL_SCANCODE_SPACE || !mBird) return;
        if (mState == State::Over) return Reset();
        if (mState == State::Ready) { mState = State::Playing; mWorld.running = true; mBird->Freeze(false); mMessage->SetVisible(false); }
        mBird->Flap();
    }

    void Update(float dt, EngineContext&) override {
        if (mState != State::Playing) return;
        if ((mSpawnTimer -= dt) <= 0.0f) {
            mSpawnTimer = kSpawnEvery;
            mActors->AddActor(std::make_unique<Pipes>(mPipeModel, mPhysics, *mActors, mWorld,
                [this] { GameOver(); }, [this] { mScore->SetText(std::to_string(++mPoints)); }));
        }
        if (Math::Abs(mBird->GetPosition().y) > kWorldTop) GameOver();
    }

private:
    enum class State { Ready, Playing, Over };

    static UIText* AddText(EngineContext& ctx, Font* font, int size, const Vector2& offset) {
        auto text = std::make_unique<UIText>(offset);
        text->SetFont(font); text->SetFontSize(size); text->SetColor(Color::White);
        return ctx.subsystems.uiElementsContext.AddText(std::move(text));
    }

    void Reset() {
        mState = State::Ready; mWorld.running = false; mWorld.round++;
        mPoints = 0; mSpawnTimer = 0.0f;
        mBird->SetPosition(Vector3{-2.0f, 0.0f, 0.0f}); mBird->Freeze(true);
        mScore->SetText("0");
        mMessage->SetText("ESPACO para voar"); mMessage->SetVisible(true);
    }

    void GameOver() {
        if (mState != State::Playing) return;
        mState = State::Over; mWorld.running = false; mBird->Freeze(true);
        mMessage->SetText("GAME OVER - ESPACO"); mMessage->SetVisible(true);
    }

    World mWorld;
    State mState{State::Ready};
    ActorsManager* mActors{nullptr};
    PhysicsSystem* mPhysics{nullptr};
    Model* mPipeModel{nullptr};
    Bird* mBird{nullptr};
    UIText* mScore{nullptr};
    UIText* mMessage{nullptr};
    float mSpawnTimer{0.0f};
    int mPoints{0};
};

int main() {
    if (Engine engine(std::make_unique<FlappyBird>()); engine.Initialize()) engine.RunLoop();
    return 0;
}
