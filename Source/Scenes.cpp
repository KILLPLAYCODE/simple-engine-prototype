#include "Scenes.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "MainScene.h"
#include "src/DATAmanager.h"
#include "ListProject.h"
#include <filesystem>
#include <vector>
#include <map>
using namespace ax;

static int _sceneID = 1003;

Scene* Scenes::createScene() 
{
    return Scenes::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "Scenes.cpp\n");
}

bool Scenes::init()
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
    std::string titleF = DATAmanager::getInstance()->titleFont;
    std::string titleB = DATAmanager::getInstance()->buttonFont;

    Color3B bgColor(135, 206, 235);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto backLayer = ui::Layout::create();
    backLayer->setContentSize(visibleSize);
    backLayer->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
    backLayer->setBackGroundColor(Color3B(0, 0, 0));
    backLayer->setBackGroundColorOpacity(198);
    backLayer->setVisible(false);
    backLayer->setPosition(Vec2::ZERO);
    backLayer->setTouchEnabled(true);
    this->addChild(backLayer,30);

    auto frontWindow = DrawNode::create();
    Color4F col = Color4F(0,0.64f,0.9f,1.0f);
    frontWindow->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200), col);
    frontWindow->setVisible(false);
    this->addChild(frontWindow,31);

    auto sceneNameLabel = Label::createWithTTF(text[5],titleF,46);
    sceneNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
    sceneNameLabel->setVisible(false);
    this->addChild(sceneNameLabel,33);

    auto buttonTextField = Sprite::create("sprites/textfield.png");
    buttonTextField->setVisible(false);
    buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
    buttonTextField->setScale(0.4f);
    auto textFieldSize = buttonTextField->getContentSize();
    this->addChild(buttonTextField,32);

    auto nameScene = ui::TextField::create(text[6],"fonts/arial.ttf",86);
    nameScene->setVisible(false);
    nameScene->setMaxLengthEnabled(true);
    nameScene->setMaxLength(18);
    nameScene->setTextColor(Color4B::BLACK);
    buttonTextField->addChild(nameScene);
    nameScene->setPosition(Vec2(textFieldSize.width/2,textFieldSize.height/2));

    auto create = ui::Button::create("sprites/button.png","sprites/button_pressed.png");
    create->setVisible(false);
    create->setPosition(Vec2(origin.x + visibleSize.width/2 + 135,origin.y + 250));
    create->setScale(0.3f);
    auto createSize = create->getContentSize();
    this->addChild(create,34);
    create->addClickEventListener([=](Object* sender){
        auto nameSc = std::string(nameScene->getString());
        auto fileUtils = FileUtils::getInstance();
        auto systemPath = DATAmanager::getInstance()->projectPath + "scenes/" + nameSc;
        if(nameSc.empty()){
            return;
        }
        if(fileUtils->isDirectoryExist(systemPath)){
            return;
        }
        fileUtils->createDirectories(systemPath);
        auto nextScene = Scenes::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    auto createLabel = Label::createWithTTF(text[0], titleB, 142);
    createLabel->setVisible(false);
    create->addChild(createLabel,36);
    createLabel->setTextColor(Color4B::BLACK);
    createLabel->setPosition(Vec2(createSize.width/2,createSize.height/2));

    auto cancel = ui::Button::create("sprites/button_1_1.png","sprites/button_1_1_pressed.png");
    cancel->setVisible(false);
    cancel->setPosition(Vec2(origin.x + visibleSize.width/2 - 135,origin.y + 250));
    cancel->setScale(0.3f);
    auto cancelSize = cancel->getContentSize();
    this->addChild(cancel,35);
    cancel->addClickEventListener([=](Object* sender){
        backLayer->setVisible(false);
    frontWindow->setVisible(false);
    buttonTextField->setVisible(false);
    nameScene->setVisible(false);
    sceneNameLabel->setVisible(false);
    create->setVisible(false);
    cancel->setVisible(false);
    createLabel->setVisible(false);
    });

    auto cancelLabel = Label::createWithTTF(text[7], titleB, 142);
    cancelLabel->setVisible(false);
    cancel->addChild(cancelLabel,36);
    cancelLabel->setTextColor(Color4B::BLACK);
    cancelLabel->setPosition(Vec2(cancelSize.width/2,cancelSize.height/2));

    auto DrawNode = DrawNode::create();
    Color4F color = Color4F(0,0.64f,0.9f,1.0f);
    DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 150),color);
    this->addChild(DrawNode,20);

    std::string name = DATAmanager::getInstance()->projectName;
    auto nameLabel = Label::createWithTTF(name,"fonts/arial.ttf",86 - name.length());
    nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 85));
    this->addChild(nameLabel, 21);

    auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    back->setScale(0.2f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + 75,visibleSize.height - 75));
    back->addClickEventListener([=](Object* sender){
        auto nextScene = ListProject::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 25);

    auto more = ui::Button::create("sprites/more.png","sprites/more_pressed.png");
    more->setScale(0.15f);
    more->setPosition(Vec2(visibleSize.width - 50,visibleSize.height - 75));
    this->addChild(more,26);

    auto listScenes = ui::ScrollView::create();
    listScenes->setContentSize(Size(visibleSize.width,visibleSize.height - 150));
    listScenes->setPosition(Vec2(origin.x,origin.y));
    listScenes->setDirection(ui::ScrollView::Direction::VERTICAL);
    listScenes->setBounceEnabled(true);
    this->addChild(listScenes);

    std::vector<std::string> sceneName;

    auto scenesPath = DATAmanager::getInstance()->projectPath + "scenes/";
try {
    for(const auto& scene : std::filesystem::directory_iterator(scenesPath)){
        if(scene.is_directory()){
            sceneName.push_back(scene.path().filename().string());
        }
    }
} catch (const std::filesystem::filesystem_error& e) {
    AXLOGE("file system error: {}", e.what());
}
float iconHeight = 232;
float total = iconHeight * sceneName.size();
if(total < visibleSize.height - 150){
        total = visibleSize.height - 150;
}
listScenes->setInnerContainerSize(Size(visibleSize.width,total));

for(int i = 0;i < sceneName.size();i++){
    float y = total - (iconHeight * i) - iconHeight/2;
    auto picture = Sprite::create("sprites/scene.png");
    picture->setPosition(Vec2(100,y));
    picture->setScale(0.15f);
    listScenes->addChild(picture);

    auto name = Label::createWithTTF(sceneName[i],"fonts/arial.ttf",68 - sceneName[i].length());
    name->setPosition(Vec2(origin.x + visibleSize.width/2,y));
    listScenes->addChild(name);

    auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
    settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
    settings->setScale(0.1f);
    listScenes->addChild(settings);

    auto next = ui::Button::create("sprites/continue_button.png","sprites/continue_button_pressed.png");
    next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
    next->setScale(0.1f);
    listScenes->addChild(next);

    /*auto icon = Sprite::create("sprites/icon_scene_1.png");
    icon->setPosition(Vec2(100,y));
    icon->setScale(0.35f);
    listScenes->addChild(icon);
*/
}
float y = total - iconHeight * sceneName.size() - iconHeight/2;
auto add = ui::Button::create("sprites/create_add.png","sprites/create_add_pressed.png");
add->setPosition(Vec2(origin.x + visibleSize.width/2,y));
add->setScale(0.25f);
listScenes->addChild(add);
add->addClickEventListener([=](Object* sender){
    backLayer->setVisible(true);
    frontWindow->setVisible(true);
    buttonTextField->setVisible(true);
    nameScene->setVisible(true);
    sceneNameLabel->setVisible(true);
    create->setVisible(true);
    cancel->setVisible(true);
    createLabel->setVisible(true);
    cancelLabel->setVisible(true);
});

scheduleUpdate();

return true;
}

void Scenes::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesBegan detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void Scenes::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesMoved detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

void Scenes::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        // AXLOGD("onTouchesEnded detected, X:{}  Y:{}", t->getLocation().x, t->getLocation().y);
    }
}

bool Scenes::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseDown detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool Scenes::onMouseUp(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOGD("onMouseUp detected, button: {}", static_cast<int>(e->getMouseButton()));
    return true;
}

bool Scenes::onMouseMove(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseMove detected, X:{}  Y:{}", e->getLocation().x, e->getLocation().y);
    return true;
}

bool Scenes::onMouseScroll(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    // AXLOGD("onMouseScroll detected, X:{}  Y:{}", e->getScrollX(), e->getScrollY());
    return true;
}

void ScenesKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("Scene: #{} onKeyPressed, keycode: {}", _sceneID, static_cast<int>(code));
}

void Scenes::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
    AXLOGD("onKeyReleased, keycode: {}", static_cast<int>(code));
}

void Scenes::update(float delta)
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

void Scenes::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}
