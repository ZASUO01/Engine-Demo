#include "Engine/Core/Cameras/CamerasManager.h"
#include "Engine/Core/Cameras/Camera.h"
#include "Engine/Graphics/Renderer/Renderer.h"

CamerasManager::CamerasManager() = default;
CamerasManager::~CamerasManager() {
    UnloadAllCameras();
}

void CamerasManager::AddCamera(std::unique_ptr<Camera> camera) {
    if (const auto it = mCameras.find(camera->mName); it == mCameras.end()) {
        mCameras[camera->mName] = std::move(camera);
    }
}

void CamerasManager::RemoveCamera(const std::string &name) {
    if (const auto it = mCameras.find(name); it != mCameras.end()) {
        mCameras.erase(it);
    }
}

void CamerasManager::UnloadAllCameras() {
    mCameras.clear();
}

void CamerasManager::SetCurrentCamera(const std::string &name) {
    if (const auto it = mCameras.find(name); it != mCameras.end()) {
        mCurrentCamera = it->second.get();
    }
}

void CamerasManager::UpdateCurrentCamera(const float deltaTime) const {
    if (mCurrentCamera && mCurrentCamera->mIsActive) {
        mCurrentCamera->Update(deltaTime);
    }
}

void CamerasManager::ProcessCurrentCameraInput(const int key) const {
    if (mCurrentCamera && mCurrentCamera->mIsActive) {
        mCurrentCamera->ProcessInput(key);
    }
}

void CamerasManager::SetRendererMatrices(Renderer *renderer) const {
    if (mCurrentCamera) {
        if (mCurrentCamera->mIsViewDirty) {
            renderer->SetViewMatrix(mCurrentCamera->mViewMatrix);
            mCurrentCamera->mIsViewDirty = false;
        }

        if (mCurrentCamera->mIsProjectionDirty) {
            renderer->SetProjectionMatrix(mCurrentCamera->mProjectionMatrix);
            mCurrentCamera->mIsProjectionDirty = false;
        }
    }
}
