#include "ListProject.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "Scenes.h"
#include "MainScene.h"
#include "src/DATAmanager.h"
#include <filesystem>
#include <map>
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif
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
void openGallery(){
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "dev/prototype/klengine/AppActivity", "OpenAndroidGallery", "()V")){
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    #endif
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

    std::map<int,std::string> text = DATAmanager::getInstance()->translater;
    //std::string marker = DATAmanager::getInstance()->markerFont;

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
    back->setPosition(Vec2(origin.x + 75,visibleSize.height - 75));
    back->addClickEventListener([=](Object* sender){
        auto nextScene = MainScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 25);

    auto label = Label::createWithTTF(text[1],"fonts/arial.ttf", 66);
    label->setPosition(Vec2(origin.x + visibleSize.width/2 + 50,visibleSize.height - 75));
    this->addChild(label, 21);

    auto scrollGroup = ui::ScrollView::create();
    scrollGroup->setContentSize(Size(visibleSize.width,visibleSize.height - 150));
    scrollGroup->setPosition(Vec2(origin.x,origin.y));
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
    scrollGroup->setInnerContainerSize(Size(visibleSize.width,totalHeight));
    for(int i = 0; i < folderNames.size();i++){
        float y = totalHeight - (i * folderHeight) - (folderHeight/2);

        std::string file = systemPath + folderNames[i] + "/icon.png";
        auto placeIcon = Sprite::create(file);
        placeIcon->setPosition(Vec2(100,y));
        placeIcon->setScale(0.25f);
        scrollGroup->addChild(placeIcon);
        touchListener->onTouchBegan = [=](Touch* touch, Event* event) -> bool {

        Vec2 touchPoint = touch->getLocation();
        Vec2 localPoint = placeIcon->convertToNodeSpace(touchPoint);
        Rect bounds = Rect(0, 0, placeIcon->getContentSize().width, placeIcon->getContentSize().height);

        if (bounds.containsPoint(localPoint)){
            auto fileUtils = FileUtils::getInstance();
            auto systemPath = fileUtils->getWritablePath();
            auto pjPath = systemPath + "projects/" + folderNames[i] + "/";
            DATAmanager::getInstance()->projectPath = pjPath;
            openGallery();
            return true;
        }
        return false;
        };

        auto item = Sprite::create("sprites/icon_scene.png");
        item->setPosition(Vec2(100,y));
        item->setScale(0.25f);
        scrollGroup->addChild(item);

        auto next = ui::Button::create("sprites/continue_button.png","sprites/continue_button_pressed.png");
        //float y = totalHeight - (i * folderHeight) - (folderHeight/2);
        next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
        next->setScale(0.1f);
        scrollGroup->addChild(next);
        next->addClickEventListener([=](Object* sender){
            auto fileUtils = FileUtils::getInstance();
            auto systemPath = fileUtils->getWritablePath();
            auto pjPath = systemPath + "projects/" + folderNames[i] + "/";
            DATAmanager::getInstance()->projectPath = pjPath;
            DATAmanager::getInstance()->projectName = folderNames[i];
            auto nextScene = Scenes::createScene();
            Director::getInstance()->replaceScene(nextScene);
        });

        auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
        settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
        settings->setScale(0.1f);
        scrollGroup->addChild(settings);

        auto nameProject = Label::createWithTTF(folderNames[i],"fonts/arial.ttf",68 - folderNames[i].length());
        nameProject->setPosition(Vec2(origin.x + visibleSize.width/2 - 25,y));
        scrollGroup->addChild(nameProject);
    }
    scheduleUpdate();

    return true;
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
    _director->end();
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
extern "C" {
    JNIEXPORT void JNICALL Java_dev_prototype_klengine_AppActivity_onImageSelectedNative(JNIEnv* env, jobject thiz, jstring path) {
        std::string cacheImagePath = JniHelper::jstring2string(path);
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([cacheImagePath](){
            auto fileUtils = FileUtils::getInstance();
            std::string filePath = DATAmanager::getInstance()->projectPath + "icon.png";
            Data fileData = fileUtils->getDataFromFile(cacheImagePath);

            if(!fileData.isNull()){
                fileUtils->writeDataToFile(file,filePath);
                fileUtils->removeFile(cacheImagePath);
                auto scene = ListProject::createScene();
                Director::getInstance()->replaceScene(scene);
            }
        });
    }
}
#endif