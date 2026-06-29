#include "Engine/Core/Actors/Actor.h"
#include <algorithm>
#include "Engine/Core/Components/Component.h"

Actor::Actor() = default;

Actor::~Actor() {
    mComponents.clear();

    for (auto* child : mChildren) {
        child->mParent = nullptr;
    }
    if (mParent) {
        mParent->RemoveChild(this);
    }
}

void Actor::SetParent(Actor *parent) {
    if (mParent) {
        mParent->RemoveChild(this);
    }
    mParent = parent;

    if (mParent) {
        mParent->AddChild(this);
    }

    SetTransformDirty();
}

void Actor::AddChild(Actor *child) {
    if (std::ranges::find(mChildren, child) == mChildren.end()) {
        child->SetUpdateOrder(GetUpdateOrder() + 1);
        mChildren.push_back(child);
    }
}

void Actor::RemoveChild(Actor *child) {
    if (const auto iter = std::ranges::find(mChildren, child); iter != mChildren.end()) {
        mChildren.erase(iter);
    }
}

void Actor::Update(const float deltaTime, Renderer *renderer, const DrawMode mode) {
    if (mState == State::Active) {
        OnUpdate(deltaTime);

        for (const auto& component : mComponents) {
            if (component->IsEnabled()) {
                component->Update(deltaTime);
                component->SendDrawCommand(renderer, mode);
                component->SendDebugDrawCommand(renderer, mode);
            }
        }
    }
}

void Actor::ProcessInput(const Uint8 *keyState) {
    if (mState == State::Active) {
        OnProcessInput(keyState);
    }
}

const Matrix4& Actor::GetWorldTransform(){
    if (mIsTransformDirty){
        mIsTransformDirty = false;

        const auto localTransform = Matrix4::CreateScale(mScale) *
                          Matrix4::CreateRotationZ(mRotation.z) *
                          Matrix4::CreateRotationX(mRotation.x) *
                          Matrix4::CreateRotationY(mRotation.y) *
                          Matrix4::CreateTranslation(mPosition);

        if (mParent) {
            mWorldTransform = localTransform * mParent->GetWorldTransform();
        }else {
            mWorldTransform = localTransform;
        }
    }

    return mWorldTransform;
}

void Actor::AddComponent(std::unique_ptr<Component> component){
    mComponents.emplace_back(std::move(component));

    std::ranges::sort(mComponents, [](const auto& a, const auto& b) {
        return a->GetUpdateOrder() < b->GetUpdateOrder();
    });
}

void Actor::SetTransformDirty() {
    if (mIsTransformDirty) {
        return;
    }

    mIsTransformDirty = true;

    for (auto* child : mChildren) {
        child->SetTransformDirty();
    }
}
