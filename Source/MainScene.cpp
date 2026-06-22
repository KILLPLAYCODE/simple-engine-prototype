//MainScene.cpp
#include "MainScene.h"
#include "ui/CocosGUI.h"
#include "CreateProj.h"
#include "ListProject.h"
#include "DATAmanager.h"
#include "language.h"
#include "axmol.h"
#include <map>
#include "Settings.h"
using namespace ax;

static int s_sceneID = 1000;

Scene* MainScene::createScene() 
{
    return MainScene::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "MainScene.cpp\n");
}

bool MainScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();
    auto safeArea    = _director->getSafeAreaRect();
    auto safeOrigin  = safeArea.origin;
    auto touchListener = EventListenerTouchOneByOne::create();
    Color3B bgColor(135, 206, 235);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    std::string l = DATAmanager::getInstance()->language;
    LANGUAGE::changeLanguage(l);

    text = DATAmanager::getInstance()->translater;
    std::string font = DATAmanager::getInstance()->buttonFont;
    float fontScale = DATAmanager::getInstance()->fontSize;
    auto label = Label::createWithTTF("Simple\nEngine", "fonts/Magic Yellow.otf", 84);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height/2 - 80));
    this->addChild(label, 2);

    auto button = ui::Button::create("sprites/button.png"sv,"sprites/button.png"sv);
    button->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 50));
    button->setScale(0.6f);
    this->addChild(button,3);
    //auto title = Label::createWithTTF(text[0],font,fontScale);
    //auto title = Label::createWithTTF(text[0], "fonts/arial.ttf",128);
    //button->addChild(title,5);
    //title->setPosition(Vec2(390,160));
    button->setTitleText(text[0]);
    button->setTitleFontName(font);
    button->setTitleFontSize(fontScale);
    button->setTitleColor(Color3B::BLACK);
    button->addClickEventListener([this](Object* sender){
        auto nextScene = CreateProj::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    auto button2 = ui::Button::create("sprites/button.png"sv,"sprites/button.png"sv);
    button2->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 - 100));
    button2->setScale(0.6f);
    this->addChild(button2,4);
    //auto title2 = Label::createWithTTF(text[1],font,fontScale - 30);
    //auto title2 = Label::createWithTTF(text[1],"fonts/arial.ttf",98);
    //button2->addChild(title2,6);
    //title2->setPosition(Vec2(390,160));
    button2->setTitleText(text[1]);
    button2->setTitleFontName(font);
    button2->setTitleFontSize(fontScale - 30);
    button2->setTitleColor(Color3B::BLACK);
    button2->addClickEventListener([this](Object* sender){
        auto nextScene = ListProject::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    auto settings = ui::Button::create("sprites/settings.png","sprites/settings_pressed.png");
    settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 275,origin.y + visibleSize.height/2 - 550));
    settings->setScale(0.3f);
    this->addChild(settings,8);
    settings->addClickEventListener([this](Object* sender){
        auto nextScene = Settings::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    auto fileUtils = FileUtils::getInstance();
    std::string systemPath = fileUtils->getWritablePath() + "projects/";
    DATAmanager::getInstance()->projectsFolder = systemPath;

    if(!fileUtils->isDirectoryExist(systemPath)){
        fileUtils->createDirectories(systemPath);
    }
    scheduleUpdate();

    return true;
}

void MainScene::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesBegan detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void MainScene::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesMoved detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void MainScene::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesEnded detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

bool MainScene::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseDown detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool MainScene::onMouseUp(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOGD("onMouseUp detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool MainScene::onMouseMove(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseMove detected, X:{}  Y:{}", e->getLocation().x, e->getLocation().y);
    return true;
}

bool MainScene::onMouseScroll(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseScroll detected, X:{}  Y:{}", e->getScrollX(), e->getScrollY());
    return true;
}

void MainScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("Scene: #{} onKeyPressed, keycode: {}", _sceneID, static_cast<int>(code));
}

void MainScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("onKeyReleased, keycode: {}", static_cast<int>(code));
}

void MainScene::update(float delta)
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

void MainScene::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}