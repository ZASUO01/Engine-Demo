#pragma once
#include <memory>
#include <unordered_map>

class Renderer;
class Camera;

class CamerasManager {
public:
    CamerasManager();
    ~CamerasManager();

    void AddCamera(std::unique_ptr<Camera> camera);
    void RemoveCamera(const std::string& name);
    void UnloadAllCameras();
    void SetCurrentCamera(const std::string& name);
private:

    void UpdateCurrentCamera(float deltaTime) const;
    void ProcessCurrentCameraInput(int key) const;
    void SetRendererMatrices(Renderer* renderer) const;

    Camera* mCurrentCamera{nullptr};
    std::unordered_map<std::string, std::unique_ptr<Camera>> mCameras;

    friend class Engine;
};