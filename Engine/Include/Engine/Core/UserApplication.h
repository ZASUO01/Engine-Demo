#pragma once

struct EngineContext;

class UserApplication {
public:
    virtual ~UserApplication() = default;

    virtual bool Init(EngineContext& ctx) = 0;
    virtual void OnResourcesReady(EngineContext& ctx) {}
    virtual void Update(float deltaTime, EngineContext& ctx) {}
    virtual void ProcessInput(int key){}
};