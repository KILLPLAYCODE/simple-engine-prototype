#include "ListProject.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "Scenes.h"
#include "MainScene.h"
#include "InsideScene.h"
#include "src/DATAmanager.h"
#include <filesystem>
#include <map>

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#include "src/portable-file-dialogs.h"
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
static void openGallery(){
    DATAmanager::getInstance()->savePath = DATAmanager::getInstance()->projectPath;
    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "dev/prototype/klengine/AppActivity", "openAndroidGallery", "()V")){
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    #elif (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
    auto select = pfd::open_file(
        "Select image",
        "",
        {"Image (*.png *.jpg *.jpeg)", "*.png *.jpg *.jpeg"},
        pfd::opt::none
    );
    if(!select.result().empty()){
        std::string fullPath = select.result()[0];
        auto fileUtils = FileUtils::getInstance();
        Data fileData = fileUtils->getDataFromFile(fullPath);
        auto spritesPath = DATAmanager::getInstance()->savePath + "icon.png";
        if(!fileData.isNull()){
            fileUtils->writeDataToFile(fileData,spritesPath);
            auto reload = ListProject::createScene();
            Director::getInstance()->replaceScene(reload);
        }
    } else {
        auto reload = ListProject::createScene();
        Director::getInstance()->replaceScene(reload);
    }
    #endif
}
void ListProject::onImported(std::string cacheName){
    auto fileUtils = FileUtils::getInstance();
    auto cachePath = DATAmanager::getInstance()->projectPath + cacheName;
    auto cacheTruePath = DATAmanager::getInstance()->projectPath + "icon.png";
    Data cacheData = fileUtils->getDataFromFile(cachePath);
    fileUtils->writeDataToFile(cacheData,cacheTruePath);
    fileUtils->removeFile(cachePath);

    auto reload = ListProject::createScene();
    Director::getInstance()->replaceScene(reload);
}
bool ListProject::init()
{
    if (!Scene::init())
    {
        return false;
    }
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();
    auto safeArea    = _director->getSafeAreaRect();
    auto safeOrigin  = safeArea.origin;

    std::map<int,std::string> text = DATAmanager::getInstance()->translater;
    std::string titleB = DATAmanager::getInstance()->buttonFont;
    std::string titleF = DATAmanager::getInstance()->titleFont;
    //std::string marker = DATAmanager::getInstance()->markerFont;
    DATAmanager::getInstance()->projectPath = "";

    std::vector<std::string> folderNames;
    auto fileUtils = FileUtils::getInstance();
    std::string projectConfig = DATAmanager::getInstance()->projectsFolder + "projects.plist";
    ValueVector projectSequance = fileUtils->getValueVectorFromFile(projectConfig);
    for(const auto& value : projectSequance){
        folderNames.push_back(value.asString());
    }

    Color3B bgColor(135, 206, 235);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto DrawNode = DrawNode::create();
    auto color = Color4F(0,0.64f,0.9f,1.0f);
    DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 150),color);
    this->addChild(DrawNode,20);

    auto option = DrawNode::create();
    option->setVisible(false);
    auto col = Color4F(0,0.64f,0.9f,1);
    option->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200),col);
    this->addChild(option,31);

    auto projectNameLabel = Label::createWithTTF(text[2],titleF,46);
    projectNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
    projectNameLabel->setVisible(false);
    this->addChild(projectNameLabel,33);

    auto buttonTextField = Sprite::create("sprites/textfield.png");
    buttonTextField->setVisible(false);
    buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
    buttonTextField->setScale(0.4f);
    auto textFieldSize = buttonTextField->getContentSize();
    this->addChild(buttonTextField,34);

    auto nameProject = ui::TextField::create(text[3],"fonts/arial.ttf",86);
    nameProject->setVisible(false);
    nameProject->setMaxLengthEnabled(true);
    nameProject->setMaxLength(18);
    nameProject->setTextColor(Color4B::BLACK);
    buttonTextField->addChild(nameProject);
    nameProject->setPosition(Vec2(textFieldSize.width/2,textFieldSize.height/2));

    auto deleteProj = ui::Button::create("sprites/button_1_1.png","sprites/button_1_1_pressed.png");
    deleteProj->setVisible(false);
    deleteProj->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + 250));
    deleteProj->setScale(0.4f);
    auto textSize = deleteProj->getContentSize();
    this->addChild(deleteProj,32);

    auto deleteLabel = Label::createWithTTF(text[8],titleB,142);
    deleteLabel->setVisible(false);
    deleteLabel->setTextColor(Color4B::BLACK);
    deleteProj->addChild(deleteLabel);
    deleteLabel->setPosition(Vec2(textSize.width/2,textSize.height/2));

    auto backLayer = ui::Layout::create();
    backLayer->setContentSize(visibleSize);
    backLayer->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
    backLayer->setBackGroundColor(Color3B(0, 0, 0));
    backLayer->setBackGroundColorOpacity(198);
    backLayer->setVisible(false);
    backLayer->setPosition(Vec2::ZERO);
    backLayer->setTouchEnabled(true);
    this->addChild(backLayer,30);
    touchListener->onTouchBegan = [=](Touch* touch, Event* event)->bool {
        if(!option->isVisible()){
            canFadeOut = false;
            return false;
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
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,option);

    backLayer->addClickEventListener([=](Object* sender){
        if(!canFadeOut){
            return;
        }
        auto fileUtils = FileUtils::getInstance();
        backLayer->setVisible(false);
        option->setVisible(false);
        deleteProj->setVisible(false);
        buttonTextField->setVisible(false);
        nameProject->setVisible(false);
        projectNameLabel->setVisible(false);
        auto oldName = DATAmanager::getInstance()->projectName;
        auto newName = std::string(nameProject->getString());
        std::string sequancePath = DATAmanager::getInstance()->projectsFolder + "projects.plist";
        if(oldName == newName){
            return;
        }
        if(newName.empty()){
            return;
        }
        auto oldPath = DATAmanager::getInstance()->projectsFolder + oldName;
        auto newPath = DATAmanager::getInstance()->projectsFolder + newName;
        fileUtils->renameFile(oldPath,newPath);
        ValueVector renameSequance = fileUtils->getValueVectorFromFile(sequancePath);

        for(int i = 0; i < renameSequance.size(); i++){
        if(renameSequance[i].asString() == oldName){
            renameSequance[i] = Value(newName);
            break;
        }
    }
        fileUtils->writeValueVectorToFile(renameSequance,sequancePath);

        auto reload = ListProject::createScene();
        Director::getInstance()->replaceScene(reload);
    });

    deleteProj->addClickEventListener([=](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        auto projectPlace = DATAmanager::getInstance()->projectsFolder + DATAmanager::getInstance()->projectName;
        fileUtils->removeDirectory(projectPlace);
        auto projectConfig = DATAmanager::getInstance()->projectsFolder + "projects.plist";
        ValueVector projects = fileUtils->getValueVectorFromFile(projectConfig);
        for(auto it = projects.begin(); it != projects.end(); ++it){
            if(it->asString() == DATAmanager::getInstance()->projectName){
                projects.erase(it);
                break;
            }
        }
        fileUtils->writeValueVectorToFile(projects,projectConfig);

        auto reload = ListProject::createScene();
        Director::getInstance()->replaceScene(reload);
    });

    auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    back->setScale(0.2f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + 50,visibleSize.height - 50));
    back->addClickEventListener([=](Object* sender){
        auto nextScene = MainScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 25);

    auto label = Label::createWithTTF(text[1],"fonts/arial.ttf", 66);
    label->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 100));
    this->addChild(label, 21);

    auto scrollGroup = ui::ScrollView::create();
    scrollGroup->setContentSize(Size(visibleSize.width,visibleSize.height - 150));
    scrollGroup->setPosition(Vec2(origin.x,origin.y));
    scrollGroup->setDirection(ui::ScrollView::Direction::VERTICAL);
    scrollGroup->setBounceEnabled(true);
    this->addChild(scrollGroup);

    std::string systemPath = DATAmanager::getInstance()->projectsFolder;

    float folderHeight = 232.0f;
    float totalHeight = folderHeight * folderNames.size();
    if(totalHeight < visibleSize.height - 150){
        totalHeight = visibleSize.height - 150;
    }
    scrollGroup->setInnerContainerSize(Size(visibleSize.width,totalHeight));
    for(int i = 0; i < folderNames.size();i++){
        float y = totalHeight - (i * folderHeight) - (folderHeight/2);

        auto fileUtils = FileUtils::getInstance();
        std::string file = systemPath + folderNames[i] + "/icon.png";
        if(!fileUtils->isFileExist(file)){
            file = "sprites/icon_scene_placeholder.png";
        }
  
        auto item = ui::Button::create(file,file);
        auto originalSize = item->getContentSize();
        float scaleX = 130 / originalSize.width;
        float scaleY = 140 / originalSize.height;
        item->setPosition(Vec2(100,y));
        //item->setScale(0.25f);
        item->setScaleX(scaleX);
        item->setScaleY(scaleY);
        scrollGroup->addChild(item);

        auto placeIcon = ui::Button::create("sprites/icon_scene.png","sprites/icon_scene.png");
        placeIcon->setPosition(Vec2(100,y));
        placeIcon->setScale(0.25f);
        scrollGroup->addChild(placeIcon);
        placeIcon->addClickEventListener([=](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        auto systemPath = fileUtils->getWritablePath();
        auto pjPath = systemPath + "projects/" + folderNames[i] + "/";
        DATAmanager::getInstance()->projectPath = pjPath;
        openGallery();
        });

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
            std::string configPj = systemPath + "projects/projects.plist";
            ValueVector pj = fileUtils->getValueVectorFromFile(configPj);
            int index = -1;
            for(int a = 0;a < pj.size();a++){
                if(pj[a].asString() == folderNames[i]){
                    index = a;
                    break;
                }
            }

            if(index > 0){
            std::swap(pj[index],pj[0]);
            fileUtils->writeValueVectorToFile(pj,configPj);
            }

            auto nextScene = Scenes::createScene();
            Director::getInstance()->replaceScene(nextScene);
        });

        auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
        settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
        settings->setScale(0.1f);
        scrollGroup->addChild(settings);
        settings->addClickEventListener([=](Object* sender){
            option->setVisible(true);
            backLayer->setVisible(true);
            deleteProj->setVisible(true);
            deleteLabel->setVisible(true);
            buttonTextField->setVisible(true);
            nameProject->setVisible(true);
            projectNameLabel->setVisible(true);
            nameProject->setString(folderNames[i]);
            DATAmanager::getInstance()->projectName = folderNames[i];
        });

        auto namePj = Label::createWithTTF(folderNames[i],"fonts/arial.ttf",68 - folderNames[i].length());
        namePj->setPosition(Vec2(origin.x + visibleSize.width/2 - 25,y));
        scrollGroup->addChild(namePj);
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
    JNIEXPORT void JNICALL Java_dev_prototype_klengine_AppActivity_onImageSelectedNative(JNIEnv* env, jobject thiz, jstring path, jstring name) {
        std::string cacheImagePath = JniHelper::jstring2string(path);
        std::string cacheName = JniHelper::jstring2string(name);
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([cacheImagePath,cacheName](){
            auto fileUtils = FileUtils::getInstance();
            std::string filePath = DATAmanager::getInstance()->savePath + cacheName;
            Data fileData = fileUtils->getDataFromFile(cacheImagePath);
            auto currentScene = Director::getInstance()->getRunningScene();
            if(!fileData.isNull()){
                fileUtils->writeDataToFile(fileData,filePath);
                fileUtils->removeFile(cacheImagePath);
                if (currentScene) {
            if (auto insideScene = dynamic_cast<InsideScene*>(currentScene)) {
                Director::getInstance()->getTextureCache()->removeTextureForKey(filePath);
                auto clothesConfig = DATAmanager::getInstance()->savePath + "clothes.plist";
                ValueVector clothesCF = fileUtils->getValueVectorFromFile(clothesConfig);
                clothesCF.push_back(Value(cacheName));
                fileUtils->writeValueVectorToFile(clothesCF,clothesConfig);
                insideScene->onImported(cacheName); 
            }
            else if (auto listProject = dynamic_cast<ListProject*>(currentScene)) {
                std::string truePath = DATAmanager::getInstance()->savePath + "icon.png";
                Director::getInstance()->getTextureCache()->removeTextureForKey(truePath);
                listProject->onImported();
            }
            }
            }
        });
    }
}
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
extern "C" {
    JNIEXPORT void JNICALL Java_dev_prototype_klengine_AppActivity_onSoundSelectedNative(JNIEnv* env, jobject thiz, jstring path, jstring name) {
        std::string cacheSoundPath = JniHelper::jstring2string(path);
        std::string cacheName = JniHelper::jstring2string(name);
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([cacheSoundPath, cacheName](){
            auto fileUtils = FileUtils::getInstance();
            std::string filePath = DATAmanager::getInstance()->savePath + cacheName;
            Data fileData = fileUtils->getDataFromFile(cacheSoundPath);
            auto currentScene = Director::getInstance()->getRunningScene();
            if(!fileData.isNull()){
                fileUtils->writeDataToFile(fileData,filePath);
                fileUtils->removeFile(cacheSoundPath);
                if (currentScene) {
            if (auto insideScene = dynamic_cast<InsideScene*>(currentScene)) {
                auto soundsConfig = DATAmanager::getInstance()->savePath + "audio.plist";
                ValueVector soundsCF = fileUtils->getValueVectorFromFile(soundsConfig);
                soundsCF.push_back(Value(fileName));
                fileUtils->writeValueVectorToFile(soundsCF,soundsConfig);
                insideScene->onImported(); 
            }
            }
            }
        });
    }
}
#endif