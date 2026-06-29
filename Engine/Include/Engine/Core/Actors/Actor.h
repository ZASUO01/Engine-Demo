#pragma once
#include <vector>

#include "Engine/Config/ConfigFlags.h"
#include "Engine/Fwd/SDLFwd.h"
#include "Engine/Math/Math.h"

class Actor{
public:
    enum class State{
        Active,
        Paused,
        Destroy
    };

    explicit Actor();
    virtual ~Actor();

    // Actor hierarchy
    void SetParent(Actor* parent);
    [[nodiscard]] Actor* GetParent() const { return mParent; }
    void AddChild(Actor* child);
    void RemoveChild(Actor* child);

    void Update(float deltaTime, class Renderer* renderer, DrawMode mode);
    void ProcessInput(const Uint8* keyState);

    Vector3 GetPosition() { return GetWorldTransform().GetTranslation(); }
    void SetPosition(const Vector3& pos) { mPosition = pos; SetTransformDirty(); }

    Vector3 GetForward() { return GetWorldTransform().GetXAxis() ;}

    [[nodiscard]] const Vector3& GetScale() const { return mScale; }
    void SetScale(const Vector3& scale) { mScale = scale; SetTransformDirty(); }

    [[nodiscard]] const Vector3& GetRotation() const { return mRotation; }
    void SetRotation(const Vector3& rotation) { mRotation = rotation; SetTransformDirty(); }

    const Matrix4& GetWorldTransform();

    [[nodiscard]] State GetState() const { return mState; }
    void SetState(const State state) { mState = state; }

    [[nodiscard]] const std::vector<std::unique_ptr<class Component>>& GetComponents() const { return mComponents; }

    template <typename T>
    T* GetComponent() const {
        for (auto& c : mComponents){
            T* t = dynamic_cast<T*>(c.get());
            if (t != nullptr){
                return t;
            }
        }

        return nullptr;
    }

    void SetUpdateOrder(const int order) { mUpdateOrder = order;}
    [[nodiscard]] int GetUpdateOrder() const { return mUpdateOrder; }

protected:
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnProcessInput(const Uint8* keyState) {}

    State mState{State::Active};

    Vector3 mPosition{Vector3::Zero};
    Vector3 mScale{Vector3::One};
    Vector3 mRotation{Vector3::Zero};

    Matrix4 mWorldTransform{Matrix4::Identity};
    bool mIsTransformDirty{true};

    void AddComponent(std::unique_ptr<Component> component);
private:
    void SetTransformDirty();

    std::vector<std::unique_ptr<Component>> mComponents;
    Actor* mParent{nullptr};
    std::vector<Actor*> mChildren;

    int mUpdateOrder{100};
};
