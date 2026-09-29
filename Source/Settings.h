#pragma once

#include "axmol.h"

class Settings : public ax::Scene
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

    CREATE_FUNC(Settings);

private:
    GameState _gameState                            = GameState::init;
    ax::EventListenerTouchAllAtOnce* _touchListener = nullptr;
    ax::EventListenerKeyboard* _keyboardListener    = nullptr;
    ax::EventListenerMouse* _mouseListener          = nullptr;
    int _sceneID                                    = 4;
    int switcher = 0;
    int switcher1 = 0;
    ax::Label* SETTINGS = nullptr;
    ax::Label* labelanguage = nullptr;
    ax::Label* mainSettings = nullptr;
    ax::Animation* idleAnim = nullptr;
    ax::Animation* idleAnim1 = nullptr;
    ax::Animation* moveAnimRight = nullptr;
    ax::Animation* moveAnimLeft = nullptr;
    //VSYNC
    bool confirmB = false;
    ax::ui::Layout* vsyncBanner = nullptr;
    ax::LayerColor* vsyncBehind = nullptr;
    ax::ui::Button* confirm = nullptr;
    ax::ui::Button* cancel = nullptr;
    ax::Label* Attention = nullptr;
    ax::Label* attention = nullptr;
};

