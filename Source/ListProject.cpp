#include "ListProject.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "Scenes.h"
#include "MainScene.h"
#include "DATAmanager.h"
#include <filesystem>
using namespace ax;

static int s_sceneID = 1001;

Scene* ListProject::createScene() 
{
    return ListProject::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "ListProject.cpp\n");
}

bool ListProject::init()
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

    Color3B bgColor(135, 206, 235);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto DrawNode = DrawNode::create();
    Color4F color = Color4F(0,0.64f,0.9f,1.0f);
    DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 150),color);
    this->addChild(DrawNode,20);

    auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    back->setScale(0.2f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + visibleSize.width/2 - backSize.width/2,origin.y + visibleSize.height - backSize.height/8));
    back->addClickEventListener([=](Object* sender){
        auto nextScene = MainScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 25);

    auto label = Label::createWithTTF("My Projects", "fonts/Marker Felt.ttf", 86);
    label->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 550));
    this->addChild(label, 21);

    auto scrollGroup = ui::ScrollView::create();
    scrollGroup->setContentSize(Size(visibleSize.width,1130));
    scrollGroup->setPosition(Vec2(origin.x + visibleSize.width/2 - 355,origin.y + visibleSize.height/2 - 640));
    scrollGroup->setDirection(ui::ScrollView::Direction::VERTICAL);
    scrollGroup->setBounceEnabled(true);
    this->addChild(scrollGroup);

    std::vector<std::string> folderNames;
    std::string systemPath = DATAmanager::getInstance()->projectsFolder;
try{
    for(const auto& entry : std::filesystem::directory_iterator(systemPath)){
        if(entry.is_directory()){
            folderNames.push_back(entry.path().filename().string());
        }
    }
} catch (const std::filesystem::filesystem_error& e) {
    AXLOGE("file system error: {}", e.what());
}
    float folderHeight = 232.0f;
    float totalHeight = folderHeight * folderNames.size();
    if(totalHeight < 1130){
        totalHeight = 1130;
    }
    scrollGroup->setInnerContainerSize(Size(720.0f,totalHeight));
    for(int i = 0; i < folderNames.size();i++){
        auto item = Sprite::create("sprites/icon_scene.png");
        float y = totalHeight - (i * folderHeight) - (folderHeight/2);
        item->setPosition(Vec2(100,y));
        item->setScale(0.25f);
        scrollGroup->addChild(item);

        auto next = ui::Button::create("sprites/continue_button.png","sprites/continue_button_pressed.png");
        //float y = totalHeight - (i * folderHeight) - (folderHeight/2);
        next->setPosition(Vec2(origin.x + visibleSize.width/2 + 220,y));
        next->setScale(0.1f);
        scrollGroup->addChild(next);
    }
    scheduleUpdate();

    return true;
}

void ListProject::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesBegan detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void ListProject::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesMoved detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void ListProject::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesEnded detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

bool ListProject::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseDown detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool ListProject::onMouseUp(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOGD("onMouseUp detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool ListProject::onMouseMove(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseMove detected, X:{}  Y:{}", e->getLocation().x, e->getLocation().y);
    return true;
}

bool ListProject::onMouseScroll(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseScroll detected, X:{}  Y:{}", e->getScrollX(), e->getScrollY());
    return true;
}

void ListProject::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("Scene: #{} onKeyPressed, keycode: {}", _sceneID, static_cast<int>(code));
}

void ListProject::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("onKeyReleased, keycode: {}", static_cast<int>(code));
}

void ListProject::update(float delta)
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

void ListProject::menuCloseCallback(ax::Object* sender)
{
    // Close the axmol game scene and quit the application
    _director->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use
     * _director->end() as given above,instead trigger a custom event created in RootViewController.mm
     * as below*/

    // EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
