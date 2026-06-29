#include "Engine/UI/UIElementsManager.h"
#include "Engine/UI/Elements/UIText.h"

UIElementsManager::UIElementsManager() = default;

UIElementsManager::~UIElementsManager() {
    UnloadAll();
}

void UIElementsManager::UnloadAll() {
    UnloadTexts();
}

UIText* UIElementsManager::AddText(std::unique_ptr<UIText> text) {
    if (text) {
        mTexts.emplace_back(std::move(text));
        return mTexts.back().get();
    }

    return nullptr;
}

void UIElementsManager::SendDrawCommands(Renderer *renderer) const {
    for (const auto& text : mTexts) {
        if (text->IsVisible()) {
            text->SendDrawCommand(renderer);
        }
    }
}

void UIElementsManager::UnloadTexts() {
    mTexts.clear();
}
