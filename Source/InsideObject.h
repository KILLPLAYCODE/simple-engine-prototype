#pragma once

#include "axmol.h"
#include "ui/CocosGUI.h"
#include <vector>
#include <string>

class InsideObject : public ax::Scene
{
    enum class GameState
    {
        init = 0,
        update,
        pause,
        end,
        menu1,
        menu2,
    };

public:
    static ax::Scene* createScene();
    bool init() override;
    void update(float delta) override;

    void menuCloseCallback(ax::Object* sender);

    CREATE_FUNC(InsideObject);

private:
    GameState _gameState                            = GameState::init;
    float cursorX = 0;
    std::vector<std::string> code;
    ax::ui::ScrollView* codeList = nullptr;
    bool capsLock = true;
};

