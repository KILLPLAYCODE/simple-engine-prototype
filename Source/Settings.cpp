#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "MainScene.h"
#include "DATAmanager.h"
#include "Settings.h"
#include <map>
#include "language.h"
using namespace ax;

static int s_sceneID = 1004;

Scene* Settings::createScene() 
{
    return Settings::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "Settings.cpp\n");
}

bool Settings::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto touchListener = EventListenerTouchOneByOne::create();
    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();
    auto safeArea    = _director->getSafeAreaRect();
    auto safeOrigin  = safeArea.origin;

    std::map<int,std::string> text = DATAmanager::getInstance()->translater;
    std::string font = DATAmanager::getInstance()->buttonFont;

    Color3B bgColor(255,43,43);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    back->setScale(0.2f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + visibleSize.width - backSize.width - 50,origin.y + visibleSize.height - backSize.height/8));
    back->addClickEventListener([=](Object* sender){
        auto nextScene = MainScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 25);

    auto languageButton = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
    languageButton->setPosition(Vec2(origin.x + visibleSize.width/2 - 200,origin.y + visibleSize.height/2));
    languageButton->setScale(0.5f);
    this->addChild(languageButton,10);
    languageButton->setTitleText("English");
    languageButton->setTitleFontName("fonts/Gokart Bubble.otf");
    languageButton->setTitleFontSize(128);
    languageButton->addClickEventListener([this](Object* sender){
        std::string l = "en";
        LANGUAGE::changeLanguage(l);
    });

    auto languageButton2 = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
    languageButton2->setPosition(Vec2(origin.x + visibleSize.width/2 + 200,origin.y + visibleSize.height/2));
    languageButton2->setScale(0.5f);
    this->addChild(languageButton2,10);
    languageButton2->setTitleText("Русский");
    languageButton2->setTitleFontName("fonts/Simple.ttf");
    languageButton2->setTitleFontSize(128);
    languageButton2->addClickEventListener([this](Object* sender){
        std::string l = "ru";
        LANGUAGE::changeLanguage(l);
    });

    auto languageButton3 = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
    languageButton3->setPosition(Vec2(origin.x + visibleSize.width/2 - 200,origin.y + visibleSize.height/2 - 150));
    languageButton3->setScale(0.5f);
    this->addChild(languageButton3,10);
    languageButton3->setTitleText("Українська");
    languageButton3->setTitleFontName("fonts/Simple.ttf");
    languageButton3->setTitleFontSize(98);
    languageButton3->addClickEventListener([this](Object* sender){
        std::string l = "ukr";
        LANGUAGE::changeLanguage(l);
    });

    scheduleUpdate();

    return true;
}

void Settings::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesBegan detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void Settings::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesMoved detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void Settings::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesEnded detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

bool Settings::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseDown detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool Settings::onMouseUp(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOGD("onMouseUp detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool Settings::onMouseMove(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseMove detected, X:{}  Y:{}", e->getLocation().x, e->getLocation().y);
    return true;
}

bool Settings::onMouseScroll(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseScroll detected, X:{}  Y:{}", e->getScrollX(), e->getScrollY());
    return true;
}

void SettingsKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("Scene: #{} onKeyPressed, keycode: {}", _sceneID, static_cast<int>(code));
}

void Settings::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("onKeyReleased, keycode: {}", static_cast<int>(code));
}

void Settings::update(float delta)
{
    switch (_gameState)
    {
    case GameState::init:
    {
        _gameState = GameState::update;
        break;
    }

    case GameState::update:
    {
        /////////////////////////////
        // Add your codes below...like....
        //
        // UpdateJoyStick();
        // UpdatePlayer();
        // UpdatePhysics();
        // ...
        break;
    }

    case GameState::pause:
    {
        /////////////////////////////
        // Add your codes below...like....
        //
        // anyPauseStuff()

        break;
    }

    case GameState::menu1:
    {  /////////////////////////////
        // Add your codes below...like....
        //
        // UpdateMenu1();
        break;
    }

    case GameState::menu2:
    {  /////////////////////////////
        // Add your codes below...like....
        //
        // UpdateMenu2();
        break;
    }

    case GameState::end:
    {  /////////////////////////////
        // Add your codes below...like....
        //
        // CleanUpMyCrap();
        menuCloseCallback(this);
        break;
    }

    }  // switch
}

void Settings::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}
