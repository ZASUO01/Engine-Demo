#pragma once

class ConfigManager;
class ShadersManager;
class AssetsManager;

class ResourcesLoader;
class Renderer;

class ActorsManager;
class CamerasManager;
class PhysicsSystem;
class Synthesizer;
class UIElementsManager;

struct ResourcesContext {
    ConfigManager& configContext;
    ShadersManager& shadersContext;
    AssetsManager& assetsContext;
};

struct OperationsContext {
    ResourcesLoader& loaderContext;
    Renderer& renderContext;
};

struct SubsystemsContext {
    ActorsManager& actorsContext;
    PhysicsSystem& physicsContext;
    CamerasManager& camerasContext;
    Synthesizer& synthContext;
    UIElementsManager& uiElementsContext;
};

struct EngineContext {
    ResourcesContext& resources;
    OperationsContext& operations;
    SubsystemsContext& subsystems;
};