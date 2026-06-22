#include "CreateProj.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "Scenes.h"
#include "MainScene.h"
#include "DATAmanager.h"
using namespace ax;

static int s_sceneID = 1001;

Scene* CreateProj::createScene() 
{
    return CreateProj::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "CreateProj.cpp\n");
}

bool CreateProj::init()
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
    std::string titleF = DATAmanager::getInstance()->titleFont;
    float fontScale = DATAmanager::getInstance()->fontSize;
    
    Color3B bgColor(135, 206, 235);
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
    this->addChild(back, 15);

    auto sprite = Sprite::create("sprites/textfield.png"sv);
    sprite->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 + 200));
    sprite->setScale(0.6f);
    Size spritesz = sprite->getContentSize();
    this->addChild(sprite,5);

    auto name = Label::createWithTTF(text[2],titleF,112);
    sprite->addChild(name,6);
    name->setPosition(Vec2(spritesz.width/2,spritesz.height + 100));

    auto button2 = ui::Button::create("sprites/button.png"sv,"sprites/button_pressed.png"sv);
    button2->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 - 100));
    button2->setScale(0.6f);
    this->addChild(button2,2);
    //auto title2 = Label::createWithTTF(text[0],font,98);
    //button2->addChild(title2,5);
    //title2->setPosition(Vec2(390,160));
    //title2->setTextColor(Color4B::BLACK);
    button2->setTitleText(text[0]);
    button2->setTitleFontName(font);
    button2->setTitleFontSize(fontScale);
    button2->setTitleColor(Color3B::BLACK);

    auto textfield = ui::TextField::create(text[3], "fonts/arial.ttf",86);
    textfield->setPosition(Vec2(spritesz.width/2,spritesz.height/2));
    textfield->setMaxLengthEnabled(true);
    textfield->setMaxLength(18);
    textfield->setTextColor(Color4B::BLACK);
    sprite->addChild(textfield,8);

    button2->addClickEventListener([=](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        std::string systemPath = fileUtils->getWritablePath();  
        std::string textfieldtxt = std::string(textfield->getString());
        AXLOGI("{}", systemPath);
        if(textfieldtxt.empty()){
            return;
        }
        std::string folder = systemPath + "projects/" + textfieldtxt + "/";
        std::string sounds = folder + "sounds/";
        std::string fonts = folder + "fonts/"; 
        if(!fileUtils->isDirectoryExist(folder)){
            fileUtils->createDirectories(folder);
            fileUtils->createDirectories(sounds);
            fileUtils->createDirectories(fonts);
        }
        if(fileUtils->isDirectoryExist(folder)){
            DATAmanager::getInstance()->projectPath = folder;
            DATAmanager::getInstance()->projectName = textfieldtxt;
            DATAmanager::getInstance()->GameScenes = 0;
            DATAmanager::getInstance()->GameObjects = 0;
            DATAmanager::getInstance()->Variables = 0;
            DATAmanager::getInstance()->Functions = 0;
            DATAmanager::getInstance()->Sounds = 0;
            DATAmanager::getInstance()->Fonts = 0;
            auto nextScene = Scenes::createScene();
        Director::getInstance()->replaceScene(nextScene);
        }
});
    scheduleUpdate();

    return true;
}

void CreateProj::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesBegan detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void CreateProj::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesMoved detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void CreateProj::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesEnded detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

bool CreateProj::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseDown detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool CreateProj::onMouseUp(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOGD("onMouseUp detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool CreateProj::onMouseMove(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseMove detected, X:{}  Y:{}", e->getLocation().x, e->getLocation().y);
    return true;
}

bool CreateProj::onMouseScroll(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseScroll detected, X:{}  Y:{}", e->getScrollX(), e->getScrollY());
    return true;
}

void CreateProj::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("Scene: #{} onKeyPressed, keycode: {}", _sceneID, static_cast<int>(code));
}

void CreateProj::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("onKeyReleased, keycode: {}", static_cast<int>(code));
}

void CreateProj::update(float delta)
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

void CreateProj::menuCloseCallback(ax::Object* sender)
{
    // Close the axmol game scene and quit the application
    _director->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use
     * _director->end() as given above,instead trigger a custom event created in RootViewController.mm
     * as below*/

    // EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
