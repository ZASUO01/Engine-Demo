#pragma once
#include <memory>
#include <vector>

class UIText;
class Renderer;

class UIElementsManager {
public:
    UIElementsManager();
    ~UIElementsManager();

    void UnloadAll();

    void SendDrawCommands(Renderer* renderer) const;
    UIText* AddText(std::unique_ptr<UIText> text);
    void UnloadTexts();

private:
    std::vector<std::unique_ptr<UIText>> mTexts;
};
