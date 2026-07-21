#pragma once

#include "axmol.h"

class CodeHelper : public ax::Scene
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

    CREATE_FUNC(CodeHelper);

private:
    GameState _gameState                            = GameState::init;
};

