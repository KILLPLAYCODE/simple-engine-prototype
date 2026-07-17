#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "MainScene.h"
#include "src/DATAmanager.h"
#include "Settings.h"
#include <map>
#include "src/language.h"
using namespace ax;

static int _sceneID = 1004;

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
    std::string LabelFont = DATAmanager::getInstance()->titleFont;

    Color3B bgColor(255,43,43);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto list = ui::ScrollView::create();
    list->setContentSize(Vec2(visibleSize.width,visibleSize.height));
    list->setPosition(Vec2(origin.x,origin.y));
    list->setDirection(ui::ScrollView::Direction::VERTICAL);
    list->setBounceEnabled(true);
    this->addChild(list);

    labelanguage = Label::createWithTTF(text[4],LabelFont,98);
    labelanguage->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 150));
    list->addChild(labelanguage, 20);

    auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    back->setScale(0.25f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + 75,visibleSize.height - 75));
    back->addClickEventListener([=](Object* sender){
        auto nextScene = MainScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    list->addChild(back, 25);

    auto languageButton = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
    languageButton->setPosition(Vec2(origin.x + visibleSize.width/2 - 175,origin.y + visibleSize.height/2));
    languageButton->setScale(0.5f);
    list->addChild(languageButton,10);
    languageButton->setTitleText("English");
    languageButton->setTitleFontName("fonts/Gokart Bubble.otf");
    languageButton->setTitleFontSize(128);
    languageButton->addClickEventListener([=](Object* sender){
        std::string l = "en";
        LANGUAGE::changeLanguage(l);
        std::string LabelFont = DATAmanager::getInstance()->titleFont;
        std::map<int,std::string> text = DATAmanager::getInstance()->translater;
        TTFConfig config(LabelFont, 98);
        labelanguage->setString(text[4]);
        labelanguage->setTTFConfig(config);
    });

    auto languageButton2 = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
    languageButton2->setPosition(Vec2(origin.x + visibleSize.width/2 + 175,origin.y + visibleSize.height/2));
    languageButton2->setScale(0.5f);
    list->addChild(languageButton2,10);
    languageButton2->setTitleText("Русский");
    languageButton2->setTitleFontName("fonts/Simple.ttf");
    languageButton2->setTitleFontSize(128);
    languageButton2->addClickEventListener([=](Object* sender){
        std::string l = "ru";
        LANGUAGE::changeLanguage(l);
        std::string LabelFont = DATAmanager::getInstance()->titleFont;
        std::map<int,std::string> text = DATAmanager::getInstance()->translater;
        TTFConfig config(LabelFont, 98);
        labelanguage->setString(text[4]);
        labelanguage->setTTFConfig(config);
    });

    auto languageButton3 = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
    languageButton3->setPosition(Vec2(origin.x + visibleSize.width/2 - 175,origin.y + visibleSize.height/2 - 150));
    languageButton3->setScale(0.5f);
    list->addChild(languageButton3,10);
    languageButton3->setTitleText("Українська");
    languageButton3->setTitleFontName("fonts/Simple.ttf");
    languageButton3->setTitleFontSize(98);
    languageButton3->addClickEventListener([=](Object* sender){
        std::string l = "ukr";
        LANGUAGE::changeLanguage(l);
        std::string LabelFont = DATAmanager::getInstance()->titleFont;
        std::map<int,std::string> text = DATAmanager::getInstance()->translater;
        TTFConfig config(LabelFont, 98);
        labelanguage->setString(text[4]);
        labelanguage->setTTFConfig(config);
    });

    scheduleUpdate();

    return true;
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
