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
#include "InsideScene.h"
#include "src/json.hpp"
#include <fstream>
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
    touchListener->setSwallowTouches(true);
    auto touchListener1 = EventListenerTouchOneByOne::create();
    touchListener1->setSwallowTouches(true);
    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();
    auto safeArea    = _director->getSafeAreaRect();
    auto safeOrigin  = safeArea.origin;

    std::map<int,std::string> text = DATAmanager::getInstance()->translater;
    std::string titleF = DATAmanager::getInstance()->titleFont;
    std::string titleB = DATAmanager::getInstance()->buttonFont;
    std::vector<std::string> sceneName;

    Color3B bgColor(135, 206, 235);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);
    //CHANGE SCENE
    auto backLayerChange = ui::Layout::create();
    backLayerChange->setContentSize(visibleSize);
    backLayerChange->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
    backLayerChange->setBackGroundColor(Color3B(0, 0, 0));
    backLayerChange->setBackGroundColorOpacity(198);
    backLayerChange->setVisible(false);
    backLayerChange->setPosition(Vec2::ZERO);
    backLayerChange->setTouchEnabled(true);
    this->addChild(backLayerChange,35);

    auto option = DrawNode::create();
    option->setVisible(false);
    auto col = Color4F(0,0.64f,0.9f,1);
    option->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200),col);
    this->addChild(option,36);

    auto SceneNameLabel = Label::createWithTTF(text[5],titleF,46);
    SceneNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
    SceneNameLabel->setVisible(false);
    this->addChild(SceneNameLabel,37);

    auto buttonTextFieldScene = Sprite::create("sprites/textfield.png");
    buttonTextFieldScene->setVisible(false);
    buttonTextFieldScene->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
    buttonTextFieldScene->setScale(0.4f);
    auto textFieldSizeScene = buttonTextFieldScene->getContentSize();
    this->addChild(buttonTextFieldScene,38);

    auto nameSceneTextField = ui::TextField::create(text[6],"fonts/arial.ttf",86);
    nameSceneTextField->setVisible(false);
    nameSceneTextField->setMaxLengthEnabled(true);
    nameSceneTextField->setMaxLength(18);
    nameSceneTextField->setTextColor(Color4B::BLACK);
    buttonTextFieldScene->addChild(nameSceneTextField);
    nameSceneTextField->setPosition(Vec2(textFieldSizeScene.width/2,textFieldSizeScene.height/2));

    auto deleteScene = ui::Button::create("sprites/button_1_1.png","sprites/button_1_1_pressed.png");
    deleteScene->setVisible(false);
    deleteScene->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + 250));
    deleteScene->setScale(0.4f);
    auto textSize = deleteScene->getContentSize();
    this->addChild(deleteScene,39);

    auto deleteLabel = Label::createWithTTF(text[8],titleB,142);
    deleteLabel->setVisible(false);
    deleteLabel->setTextColor(Color4B::BLACK);
    deleteScene->addChild(deleteLabel);
    deleteLabel->setPosition(Vec2(textSize.width/2,textSize.height/2));

    touchListener1->onTouchBegan = [=](Touch* touch, Event* event)->bool {
        if(!option->isVisible()){
            return false;
        }
        if(canFadeOut){
            canFadeOut = false;
        }
            Vec2 localPoint = option->convertToNodeSpace(touch->getLocation());
            Rect bounds = Rect(origin.x + 100,origin.y + 200, visibleSize.width - 200,visibleSize.height - 400);

        if(bounds.containsPoint(localPoint)){
            canFadeOut = false;
            return true;
        }
        canFadeOut = true;
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener1,option);

    backLayerChange->addClickEventListener([=](Object* sender){
        if(!canFadeOut){
            AXLOGW("FAILED");
            return;
        }
        auto fileUtils = FileUtils::getInstance();
        backLayerChange->setVisible(false);
        option->setVisible(false);
        deleteScene->setVisible(false);
        buttonTextFieldScene->setVisible(false);
        nameSceneTextField->setVisible(false);
        SceneNameLabel->setVisible(false);
        auto oldName = DATAmanager::getInstance()->sceneName;
        auto newName = std::string(nameSceneTextField->getString());
        if(oldName == newName){
            return;
        }
        if(newName.empty()){
            return;
        }
        auto oldPath = DATAmanager::getInstance()->projectsFolder + DATAmanager::getInstance()->projectName + "/scenes/" + oldName;
        auto newPath = DATAmanager::getInstance()->projectsFolder + DATAmanager::getInstance()->projectName + "/scenes/" + newName;
        fileUtils->renameFile(oldPath,newPath);
        auto reload = Scenes::createScene();
        Director::getInstance()->replaceScene(reload);
    });
    deleteScene->addClickEventListener([=](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        auto projectPlace = DATAmanager::getInstance()->projectsFolder + DATAmanager::getInstance()->projectName + "/scenes/" + DATAmanager::getInstance()->sceneName;
        fileUtils->removeDirectory(projectPlace);
        auto reload = Scenes::createScene();
        Director::getInstance()->replaceScene(reload);
    });
    //CREATE SCENE
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
    auto color1 = Color4F(0,0.64f,0.9f,1.0f);
    frontWindow->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200), color1);
    frontWindow->setVisible(false);
    this->addChild(frontWindow,31);
    touchListener->onTouchBegan = [=](Touch* touch, Event* event)->bool {
        if(!frontWindow->isVisible()){
            return false;
        }
        if(canFadeOut){
            canFadeOut = false;
        }
            Vec2 localPoint = frontWindow->convertToNodeSpace(touch->getLocation());
            Rect bounds = Rect(origin.x + 100,origin.y + 200, visibleSize.width - 200,visibleSize.height - 400);

        if(bounds.containsPoint(localPoint)){
            canFadeOut = false;
            return true;
        }
        canFadeOut = true;
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,frontWindow);

    auto sceneNameLabel = Label::createWithTTF(text[5],titleF,46);
    sceneNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
    sceneNameLabel->setVisible(false);
    this->addChild(sceneNameLabel,33);

    auto buttonTextField = Sprite::create("sprites/textfield.png");
    buttonTextField->setVisible(false);
    buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
    buttonTextField->setScale(0.4f);
    auto textFieldSize = buttonTextFieldScene->getContentSize();
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

    auto cancelLabel = Label::createWithTTF(text[7], titleB, 142);
    cancelLabel->setVisible(false);
    cancel->addChild(cancelLabel,36);
    cancelLabel->setTextColor(Color4B::BLACK);
    cancelLabel->setPosition(Vec2(cancelSize.width/2,cancelSize.height/2));

    backLayer->addClickEventListener([=](Object* sender){
        if(!canFadeOut){
            return;
        }
        backLayer->setVisible(false);
        frontWindow->setVisible(false);
        buttonTextField->setVisible(false);
        nameScene->setVisible(false);
        sceneNameLabel->setVisible(false);
        create->setVisible(false);
        cancel->setVisible(false);
        createLabel->setVisible(false);
    });

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
    //UI

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

    auto run = ui::Button::create("sprites/button_run.png","sprites/button_run_pressed.png");    
    run->setPosition(Vec2(visibleSize.width - 125,origin.y + 100));
    run->setScale(0.25f);
    this->addChild(run,12);

    auto add = ui::Button::create("sprites/create_add.png","sprites/create_add_pressed.png");
    add->setPosition(Vec2(visibleSize.width - 125,origin.y + 250));
    add->setScale(0.25f);
    this->addChild(add,13);
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

    auto listScenes = ui::ScrollView::create();
    listScenes->setContentSize(Size(visibleSize.width,visibleSize.height - 150));
    listScenes->setPosition(Vec2(origin.x,origin.y));
    listScenes->setDirection(ui::ScrollView::Direction::VERTICAL);
    listScenes->setBounceEnabled(true);
    this->addChild(listScenes);

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
    settings->addClickEventListener([=](Object* sender){
            option->setVisible(true);
            backLayerChange->setVisible(true);
            deleteScene->setVisible(true);
            deleteLabel->setVisible(true);
            buttonTextFieldScene->setVisible(true);
            nameSceneTextField->setVisible(true);
            SceneNameLabel->setVisible(true);
            nameSceneTextField->setString(sceneName[i]);
            DATAmanager::getInstance()->sceneName = sceneName[i];
    });

    auto up = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
    up->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y + 50));
    up->setScale(0.1f);
    listScenes->addChild(up);

    auto down = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
    down->setRotation(180);
    down->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y - 50));
    down->setScale(0.1f);
    listScenes->addChild(down);

    auto next = ui::Button::create("sprites/continue_button.png","sprites/continue_button_pressed.png");
    next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
    next->setScale(0.1f);
    listScenes->addChild(next);
    next->addClickEventListener([=](Object* sender){
        DATAmanager::getInstance()->sceneName = sceneName[i];
        auto nextScene = InsideScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
}
scheduleUpdate();

return true;
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
