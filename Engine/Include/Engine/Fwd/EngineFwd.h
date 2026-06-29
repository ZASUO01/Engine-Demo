#pragma once
#include <memory>

// Context
struct EngineContext;
enum class DrawMode : uint32_t;

// Resources Context
class ConfigManager;
using UniqueConfigManager = std::unique_ptr<ConfigManager>;

class ShadersManager;
using UniqueShadersManager = std::unique_ptr<ShadersManager>;

class AssetsManager;
using UniqueAssetsManager = std::unique_ptr<AssetsManager>;

// Operations Context
class Renderer;
using UniqueRenderer = std::unique_ptr<Renderer>;

class ResourcesLoader;
using UniqueResourcesLoader = std::unique_ptr<ResourcesLoader>;

//Systems Context
class CamerasManager;
using UniqueCamerasManager = std::unique_ptr<CamerasManager>;

class ActorsManager;
using UniqueActorsManager = std::unique_ptr<ActorsManager>;

class PhysicsSystem;
using UniquePhysicsSystem = std::unique_ptr<PhysicsSystem>;

// Application
class UserApplication;
using UniqueUserApp = std::unique_ptr<UserApplication>;

class GlobalLight;
using UniqueGlobalLight = std::unique_ptr<GlobalLight>;

class Synthesizer;
using UniqueSynthesizer = std::unique_ptr<Synthesizer>;

class UIElementsManager;
using UniqueUIElementsManager = std::unique_ptr<UIElementsManager>;