#include "InsideScene.h"
#include "Scenes.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "audio/AudioEngine.h"
#include "MainScene.h"
#include "ListProject.h"
#include "src/DATAmanager.h"
#include "InsideObject.h"
#include <filesystem>
#include <vector>
#include <map>
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#include "src/portable-file-dialogs.h"
#endif
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#include "src/BridgeIOS.h"
#endif

using namespace ax;

static int _sceneID = 1005;

Scene* InsideScene::createScene() 
{
    return InsideScene::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "InsideScene.cpp\n");
}
static void openGallery(){ //IMPORT IMAGE
    DATAmanager::getInstance()->savePath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + DATAmanager::getInstance()->objectName + "/clothes/";

    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) //ANDROID
    JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "dev/prototype/klengine/AppActivity", "openAndroidGallery", "()V")){
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    #elif (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) //WINDOWS, LINUX
    auto select = pfd::open_file(
        "Select image",
        "",
        {"Image (*.png *.jpg *.jpeg)", "*.png *.jpg *.jpeg"},
        pfd::opt::none
    );
    if(!select.result().empty()){
        std::string fullPath = select.result()[0];
        std::filesystem::path p(fullPath);
        std::string fileName = p.filename().string();
        auto fileUtils = FileUtils::getInstance();
        Data fileData = fileUtils->getDataFromFile(fullPath);
        auto spritesPath = DATAmanager::getInstance()->savePath + fileName;
        auto clothesConfig = DATAmanager::getInstance()->savePath + "clothes.plist";
        ValueVector clothesCF = fileUtils->getValueVectorFromFile(clothesConfig);
        clothesCF.push_back(Value(fileName));
        fileUtils->writeValueVectorToFile(clothesCF,clothesConfig);
        if(!fileData.isNull()){
            fileUtils->writeDataToFile(fileData,spritesPath);
            auto reload = InsideScene::createScene();
            Director::getInstance()->replaceScene(reload);
        }
    } else {
        auto reload = InsideScene::createScene();
        Director::getInstance()->replaceScene(reload);
    }
    #elif (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) //IOS
    DATAmanager::getInstance()->iosRequestSender = 2;
    IOSBridge::importMediaFile("image");
    #endif
}
static void openSound(){
    DATAmanager::getInstance()->savePath = DATAmanager::getInstance()->projectPath + "sounds/";

    #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) //ANDROID
    JniMethodInfo methodInfo;
    if(JniHelper::getStaticMethodInfo(methodInfo, "dev/prototype/klengine/AppActivity", "openAudioFounder", "()V")){
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    #elif (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) //WIN,LINUX
    auto select = pfd::open_file(
        "Select sound",
        "",
        {"Sound (*.mp3 *.ogg *.wav)", "*.mp3 *.ogg *.wav"},
        pfd::opt::none
    );
    if(!select.result().empty()){
        std::string fullPath = select.result()[0];
        std::filesystem::path p(fullPath);
        std::string fileName = p.filename().string();
        auto fileUtils = FileUtils::getInstance();
        Data fileData = fileUtils->getDataFromFile(fullPath);
        auto soundsPath = DATAmanager::getInstance()->savePath + fileName;
        auto soundsConfig = DATAmanager::getInstance()->savePath + "audio.plist";
        ValueVector soundsCF = fileUtils->getValueVectorFromFile(soundsConfig);
        soundsCF.push_back(Value(fileName));
        fileUtils->writeValueVectorToFile(soundsCF,soundsConfig);
        if(!fileData.isNull()){
            fileUtils->writeDataToFile(fileData,soundsPath);
            auto reload = InsideScene::createScene();
            Director::getInstance()->replaceScene(reload);
        }
    } else {
        auto reload = InsideScene::createScene();
        Director::getInstance()->replaceScene(reload);
    }
    #elif (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) //IOS
    if(DATAmanager::getInstance()->iosRequestSender == 0){
        DATAmanager::getInstance()->iosRequestSender = 2;
        IOSBridge::importMediaFile("audio");
    }
    #endif
}
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
void IOSBridge::onFileSelected(const std::string& cachePath, std::string fileName){
    if(DATAmanager::getInstance()->iosRequestSender = 1){ //LISTPROJECT
        auto spritesPath = DATAmanager::getInstance()->savePath + "icon.png";
        auto fileUtils = FileUtils::getInstance();
        Data cacheData = fileUtils->getDataFromFile(cachePath);
        fileUtils->writeDataToFile(cacheData,spritesPath);
        fileUtils->removeFile(cachePath);
        auto currentScene = Director::getInstance()->getRunningScene();
        auto listProject = dynamic_cast<ListProject*>(currentScene);
        listProject->onImported(fileName);
    }
    if(DATAmanager::getInstance()->iosRequestSender = 2){ //INSIDESCENE
        uint8_t type = DATAmanager::getInstance()->switcher;
    if(type == 0){ //IMAGE
        auto spritesPath = DATAmanager::getInstance()->savePath + fileName;
        auto fileUtils = FileUtils::getInstance();
        Data cacheData = fileUtils->getDataFromFile(cachePath);
        if(!cacheData.isNull()){
            fileUtils->writeDataToFile(cacheData,spritesPath);
            fileUtils->removeFile(cachePath);    
            Director::getInstance()->getTextureCache()->removeTextureForKey(spritesPath);
        }
        auto clothesConfig = DATAmanager::getInstance()->savePath + "clothes.plist";
        ValueVector clothesCF = fileUtils->getValueVectorFromFile(clothesConfig);
        clothesCF.push_back(Value(fileName));
        fileUtils->writeValueVectorToFile(clothesCF,clothesConfig);

        auto reload = InsideObject::createScene();
        Director::getInstance()->replaceScene(reload);
    }
    if(type == 1){ //AUDIO
        auto soundsPath = DATAmanager::getInstance()->savePath + fileName;
        auto fileUtils = FileUtils::getInstance();
        Data cacheData = fileUtils->getDataFromFile(cachePath);
        if(cacheData.isNull()){
            return;
        }
        fileUtils->writeDataToFile(cacheData,soundsPath);
        fileUtils->removeFile(cachePath);    
        auto soundsConfig = DATAmanager::getInstance()->savePath + "audio.plist";
        ValueVector soundsCF = fileUtils->getValueVectorFromFile(soundsConfig);
        soundsCF.push_back(Value(fileName));
        fileUtils->writeValueVectorToFile(soundsCF,soundsConfig);

        auto reload = InsideScene::createScene();
        Director::getInstance()->replaceScene(reload);
    }
    DATAmanager::getInstance()->iosRequestSender = 0;
    }
}
#endif

void InsideScene::onImported(){
    auto reload = InsideScene::createScene();
    Director::getInstance()->replaceScene(reload);
}

bool InsideScene::init()
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
    uint8_t category = DATAmanager::getInstance()->switcher;
    switch(category)
    {
        case 0:
        {
            std::vector<std::string> objectsName;
            auto fileUtils = FileUtils::getInstance();
            std::string configSequance = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/objects.plist";
            if(fileUtils->isFileExist(configSequance)){
                ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                for(const auto& value : sequance){
                    objectsName.push_back(value.asString());
                }  
            }

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

            auto objectNameLabel = Label::createWithTTF(text[9],titleF,46);    
            objectNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
            objectNameLabel->setVisible(false);
            this->addChild(objectNameLabel,33);
    
            auto buttonTextField = Sprite::create("sprites/textfield.png");
            buttonTextField->setVisible(false);
            buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
            buttonTextField->setScale(0.4f);
            auto textFieldSize = buttonTextField->getContentSize();
            this->addChild(buttonTextField,32);

            auto deleteObject = ui::Button::create("sprites/button_1_1.png","sprites/button_1_1_pressed.png");
            deleteObject->setVisible(false);
            deleteObject->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + 250));
            deleteObject->setScale(0.4f);
            auto textSize = deleteObject->getContentSize();
            this->addChild(deleteObject,39);
            deleteObject->addClickEventListener([=](Object* sender){
                auto fileUtils = FileUtils::getInstance();
                auto objectPlace = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + DATAmanager::getInstance()->objectName;
                fileUtils->removeDirectory(objectPlace);

                std::string objectsConfig = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/objects.plist";
                ValueVector sequance = fileUtils->getValueVectorFromFile(objectsConfig);
                for(auto it = sequance.begin();it != sequance.end();++it){
                    if(it->asString() == DATAmanager::getInstance()->sceneName){
                        sequance.erase(it);
                        break;
                    }
                }
                fileUtils->writeValueVectorToFile(sequance,objectsConfig);

                auto reload = InsideScene::createScene();        
                Director::getInstance()->replaceScene(reload);
            });

            auto deleteLabel = Label::createWithTTF(text[8],titleB,142);
            deleteLabel->setVisible(false);
            deleteLabel->setTextColor(Color4B::BLACK);
            deleteObject->addChild(deleteLabel);
            deleteLabel->setPosition(Vec2(textSize.width/2,textSize.height/2));

            auto nameObject = ui::TextField::create(text[10],"fonts/arial.ttf",86);    
            nameObject->setVisible(false);
            nameObject->setMaxLengthEnabled(true);
            nameObject->setMaxLength(18);
            nameObject->setTextColor(Color4B::BLACK);
            buttonTextField->addChild(nameObject);
            nameObject->setPosition(Vec2(textFieldSize.width/2,textFieldSize.height/2));

            auto create = ui::Button::create("sprites/button.png","sprites/button_pressed.png");    
            create->setVisible(false);
            create->setPosition(Vec2(origin.x + visibleSize.width/2 + 135,origin.y + 250));
            create->setScale(0.3f);
            auto createSize = create->getContentSize();
            this->addChild(create,34);
            create->addClickEventListener([=](Object* sender){
                auto nameSc = std::string(nameObject->getString());
                DATAmanager::getInstance()->objectName = nameSc;
                auto fileUtils = FileUtils::getInstance();
                auto systemPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + nameSc + "/";
                if(nameSc.empty()){
                    return;
                }
                if(fileUtils->isDirectoryExist(systemPath)){
                    return;
                }
                auto clothesPath = systemPath + "clothes/";
                auto clothesConfig = clothesPath + "clothes.plist";
                auto script = systemPath + "code.cx";
                std::string start = "print(text(Hello, world!))";
                ValueVector clothes;
                fileUtils->createDirectories(systemPath);
                fileUtils->createDirectories(clothesPath);
                fileUtils->writeValueVectorToFile(clothes,clothesConfig);
                fileUtils->writeStringToFile(start,script);

                std::string objectConfig = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/objects.plist";
                ValueVector saveObjects = fileUtils->getValueVectorFromFile(objectConfig);
                saveObjects.push_back(Value(nameSc));
                fileUtils->writeValueVectorToFile(saveObjects,objectConfig);
                openGallery();
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
                nameObject->setVisible(false);
                objectNameLabel->setVisible(false);
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

            float xDuration = 175;    
            for(int i = 1;i < 5;i++){
                float x = xDuration * i;
                if(i == 1){
                    auto microButton = ui::Button::create("sprites/button_2_0.png","sprites/button_2_0.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);

                    auto objectsIcon = Sprite::create("sprites/icon_objects.png");            
                    objectsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    objectsIcon->setScale(0.08f);
                    this->addChild(objectsIcon,15 + i);
                }
                if(i == 2){
                    auto microButton = ui::Button::create("sprites/button_2_1.png","sprites/button_2_1.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 1;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });
            
                    auto soundsIcon = Sprite::create("sprites/icon_sound.png");
                    soundsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    soundsIcon->setScale(0.08f);
                    this->addChild(soundsIcon,15 + i);
                }
                if(i == 3){
                    auto microButton = ui::Button::create("sprites/button_2_3.png","sprites/button_2_3.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 2;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto fontsIcon = Sprite::create("sprites/icon_fonts.png");            
                    fontsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    fontsIcon->setScale(0.1f);
                    this->addChild(fontsIcon,15 + i);
                }
                if(i == 4){
                    auto microButton = ui::Button::create("sprites/button_2_2.png","sprites/button_2_2.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 3;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto texturesIcon = Sprite::create("sprites/icon_textures.png");            
                    texturesIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    texturesIcon->setScale(0.08f);
                    this->addChild(texturesIcon,15 + i);
                }
            }
            backLayer->addClickEventListener([=](Object* sender){
        if(!canFadeOut){
            return;
        }
        auto fileUtils = FileUtils::getInstance();
        backLayer->setVisible(false);
        frontWindow->setVisible(false);
        deleteObject->setVisible(false);
        buttonTextField->setVisible(false);
        nameObject->setVisible(false);
        objectNameLabel->setVisible(false);
        create->setVisible(false);
        cancel->setVisible(false);
        createLabel->setVisible(false);
        auto oldName = DATAmanager::getInstance()->objectName;
        auto newName = std::string(nameObject->getString());
        std::string sequancePath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/objects.plist";
        if(oldName == newName){
            return;
        }
        if(newName.empty()){
            return;
        }
        auto oldPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + oldName;
        auto newPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + newName;
        fileUtils->renameFile(oldPath,newPath);
        ValueVector renameSequance = fileUtils->getValueVectorFromFile(sequancePath);

        for(int i = 0; i < renameSequance.size(); i++){
        if(renameSequance[i].asString() == oldName){
            renameSequance[i] = Value(newName);
            break;
        }
    }
        fileUtils->writeValueVectorToFile(renameSequance,sequancePath);

        auto reload = InsideScene::createScene();
        Director::getInstance()->replaceScene(reload);
    });

            std::string name = DATAmanager::getInstance()->sceneName;    
            auto nameLabel = Label::createWithTTF(name,"fonts/arial.ttf",86 - name.length() * 2);
            nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 85));
            this->addChild(nameLabel, 21);
    
            auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
            back->setScale(0.2f);
            auto backSize = back->getContentSize();
            back->setPosition(Vec2(origin.x + 50,visibleSize.height - 50));
            back->addClickEventListener([=](Object* sender){
                auto nextScene = Scenes::createScene();
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
                nameObject->setVisible(true);
                objectNameLabel->setVisible(true);
                create->setVisible(true);
                cancel->setVisible(true);
                createLabel->setVisible(true);
                cancelLabel->setVisible(true);
            });

            auto listObjects = ui::ScrollView::create();    
            listObjects->setContentSize(Size(visibleSize.width,visibleSize.height - 225));
            listObjects->setPosition(Vec2(origin.x,origin.y));
            listObjects->setDirection(ui::ScrollView::Direction::VERTICAL);
            listObjects->setBounceEnabled(true);
            this->addChild(listObjects);
    
            auto objectsPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/";
            float iconHeight = 232;
            float total = iconHeight * objectsName.size();
            if(total < visibleSize.height - 225){
                total = visibleSize.height - 225;
            }
            listObjects->setInnerContainerSize(Size(visibleSize.width,total));

            for(int i = 0;i < objectsName.size();i++){
                auto fileUtils = FileUtils::getInstance();
                float y = total - (iconHeight * i) - iconHeight/2;
                auto clothesConfig = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + objectsName[i] + "/clothes/clothes.plist";
                ValueVector cloth = fileUtils->getValueVectorFromFile(clothesConfig);
                std::string spritesPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + objectsName[i] + "/clothes/" + cloth[0].asString();
                if(!fileUtils->isFileExist(spritesPath)){
                    spritesPath = "sprites/null.png";
                }
                auto picture = Sprite::create(spritesPath);
                picture->setPosition(Vec2(100,y));
                //picture->setScale(0.15f);
                auto originalSize = picture->getContentSize();
                float scaleX = 130 / originalSize.width;
                float scaleY = 135 / originalSize.height;
                picture->setScaleX(scaleX);
                picture->setScaleY(scaleY);
                listObjects->addChild(picture);

                auto name = Label::createWithTTF(objectsName[i],"fonts/arial.ttf",68 - objectsName[i].length() * 2.5f);
                name->setPosition(Vec2(origin.x + visibleSize.width/2,y));
                listObjects->addChild(name);

                auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
                settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
                settings->setScale(0.1f);
                listObjects->addChild(settings);
                settings->addClickEventListener([=](Object* sender){
                    frontWindow->setVisible(true);
                    backLayer->setVisible(true);
                    deleteObject->setVisible(true);
                    deleteLabel->setVisible(true);
                    buttonTextField->setVisible(true);
                    nameObject->setVisible(true);
                    objectNameLabel->setVisible(true);
                    nameObject->setString(objectsName[i]);            
                    DATAmanager::getInstance()->objectName = objectsName[i];
                });

                auto next = ui::Button::create("sprites/continue_button.png","sprites/continue_button_pressed.png");
                next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                next->setScale(0.1f);
                listObjects->addChild(next);
                next->addClickEventListener([=](Object* sender){
                    DATAmanager::getInstance()->objectName = objectsName[i];
                    auto nextScene = InsideObject::createScene();
                    Director::getInstance()->replaceScene(nextScene);
                });

                auto icon = Sprite::create("sprites/icon_scene_1.png");
                icon->setPosition(Vec2(100,y));
                icon->setScale(0.25f);
                listObjects->addChild(icon);

                auto up = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                up->setPosition(Vec2(origin.x + visibleSize.width/2 - 125,y + 50));
                up->setScale(0.1f);
                listObjects->addChild(up);
                up->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/objects.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == objectsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index > 0){
                        std::swap(sequance[index],sequance[index - 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
                auto down = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                down->setRotation(180);
                down->setPosition(Vec2(origin.x + visibleSize.width/2 - 125,y - 50));
                down->setScale(0.1f);
                listObjects->addChild(down);
                down->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/objects.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
    
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == objectsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index < sequance.size() - 1){
                        std::swap(sequance[index],sequance[index + 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
            }
            break;
        }
        case 1:
        {
            std::vector<std::string> soundsName;
            auto fileUtils = FileUtils::getInstance();
            std::string configSequance = DATAmanager::getInstance()->projectPath + "sounds/audio.plist";
            if(fileUtils->isFileExist(configSequance)){
                ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                for(const auto& value : sequance){
                    soundsName.push_back(value.asString());
                }  
            }

            Color3B bgColor(156, 128, 247);
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
            Color4F col = Color4F(0.59f,0,0.71f,1.0f);
            frontWindow->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200), col);
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

            auto objectNameLabel = Label::createWithTTF(text[9],titleF,46);    
            objectNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
            objectNameLabel->setVisible(false);
            this->addChild(objectNameLabel,33);
    
            auto buttonTextField = Sprite::create("sprites/textfield.png");
            buttonTextField->setVisible(false);
            buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
            buttonTextField->setScale(0.4f);
            auto textFieldSize = buttonTextField->getContentSize();
            this->addChild(buttonTextField,32);

            auto deleteObject = ui::Button::create("sprites/button_1_1.png","sprites/button_1_1_pressed.png");
            deleteObject->setVisible(false);
            deleteObject->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + 250));
            deleteObject->setScale(0.4f);
            auto textSize = deleteObject->getContentSize();
            this->addChild(deleteObject,39);
            deleteObject->addClickEventListener([=](Object* sender){
                auto fileUtils = FileUtils::getInstance();
                auto soundPlace = DATAmanager::getInstance()->projectPath + "sounds/" + DATAmanager::getInstance()->soundName;
                fileUtils->removeDirectory(soundPlace);

                std::string objectsConfig = DATAmanager::getInstance()->projectPath + "sounds/audio.plist";
                ValueVector sequance = fileUtils->getValueVectorFromFile(objectsConfig);
                for(auto it = sequance.begin();it != sequance.end();++it){
                    if(it->asString() == DATAmanager::getInstance()->soundName){
                        sequance.erase(it);
                        break;
                    }
                }
                fileUtils->writeValueVectorToFile(sequance,objectsConfig);

                auto reload = InsideScene::createScene();        
                Director::getInstance()->replaceScene(reload);
            });

            auto deleteLabel = Label::createWithTTF(text[8],titleB,142);
            deleteLabel->setVisible(false);
            deleteLabel->setTextColor(Color4B::BLACK);
            deleteObject->addChild(deleteLabel);
            deleteLabel->setPosition(Vec2(textSize.width/2,textSize.height/2));

            auto nameObject = ui::TextField::create(text[10],"fonts/arial.ttf",86);    
            nameObject->setVisible(false);
            nameObject->setMaxLengthEnabled(true);
            nameObject->setMaxLength(64);
            nameObject->setTextColor(Color4B::BLACK);
            buttonTextField->addChild(nameObject);
            nameObject->setPosition(Vec2(textFieldSize.width/2,textFieldSize.height/2));

            auto DrawNode = DrawNode::create();    
            Color4F color = Color4F(0.59f,0,0.71f,1.0f);
            DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 150),color);
            this->addChild(DrawNode,20);

            float xDuration = 175;    
            for(int i = 1;i < 5;i++){
                float x = xDuration * i;
                if(i == 1){
                    auto microButton = ui::Button::create("sprites/button_2_0.png","sprites/button_2_0.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 0;
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto objectsIcon = Sprite::create("sprites/icon_objects.png");            
                    objectsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    objectsIcon->setScale(0.08f);
                    this->addChild(objectsIcon,15 + i);
                }
                if(i == 2){
                    auto microButton = ui::Button::create("sprites/button_2_1.png","sprites/button_2_1.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
            
                    auto soundsIcon = Sprite::create("sprites/icon_sound.png");
                    soundsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    soundsIcon->setScale(0.08f);
                    this->addChild(soundsIcon,15 + i);
                }
                if(i == 3){
                    auto microButton = ui::Button::create("sprites/button_2_3.png","sprites/button_2_3.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 2;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto fontsIcon = Sprite::create("sprites/icon_fonts.png");            
                    fontsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    fontsIcon->setScale(0.1f);
                    this->addChild(fontsIcon,15 + i);
                }
                if(i == 4){
                    auto microButton = ui::Button::create("sprites/button_2_2.png","sprites/button_2_2.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 3;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto texturesIcon = Sprite::create("sprites/icon_textures.png");            
                    texturesIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    texturesIcon->setScale(0.08f);
                    this->addChild(texturesIcon,15 + i);
                }
            }
            backLayer->addClickEventListener([=](Object* sender){
        if(!canFadeOut){
            return;
        }
        auto fileUtils = FileUtils::getInstance();
        backLayer->setVisible(false);
        frontWindow->setVisible(false);
        deleteObject->setVisible(false);
        buttonTextField->setVisible(false);
        nameObject->setVisible(false);
        objectNameLabel->setVisible(false);
        auto oldName = DATAmanager::getInstance()->soundName;
        auto newName = std::string(nameObject->getString());
        std::string sequancePath = DATAmanager::getInstance()->projectPath + "sounds/" + "/audio.plist";
        if(oldName == newName){
            return;
        }
        if(newName.empty()){
            return;
        }
        auto oldPath = DATAmanager::getInstance()->projectPath + "sounds/" + oldName;
        auto newPath = DATAmanager::getInstance()->projectPath + "sounds/" + newName;
        fileUtils->renameFile(oldPath,newPath);
        ValueVector renameSequance = fileUtils->getValueVectorFromFile(sequancePath);

        for(int i = 0; i < renameSequance.size(); i++){
        if(renameSequance[i].asString() == oldName){
            renameSequance[i] = Value(newName);
            break;
        }
    }
        fileUtils->writeValueVectorToFile(renameSequance,sequancePath);
        auto reload = InsideScene::createScene();
        Director::getInstance()->replaceScene(reload);
    });

            std::string name = DATAmanager::getInstance()->sceneName;    
            auto nameLabel = Label::createWithTTF(name,"fonts/arial.ttf",86 - name.length() * 2);
            nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 85));
            this->addChild(nameLabel, 21);
    
            auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
            back->setScale(0.2f);
            auto backSize = back->getContentSize();
            back->setPosition(Vec2(origin.x + 50,visibleSize.height - 50));
            back->addClickEventListener([=](Object* sender){
                AudioEngine::stopAll();
                DATAmanager::getInstance()->sound_id = 0;
                DATAmanager::getInstance()->activePlayButton = nullptr;
                DATAmanager::getInstance()->activeStopButton = nullptr;
                auto nextScene = Scenes::createScene();
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
                AudioEngine::stopAll();
                DATAmanager::getInstance()->sound_id = 0;
                DATAmanager::getInstance()->activePlayButton = nullptr;
                DATAmanager::getInstance()->activeStopButton = nullptr;
                openSound();
            });

            auto listObjects = ui::ScrollView::create();    
            listObjects->setContentSize(Size(visibleSize.width,visibleSize.height - 225));
            listObjects->setPosition(Vec2(origin.x,origin.y));
            listObjects->setDirection(ui::ScrollView::Direction::VERTICAL);
            listObjects->setBounceEnabled(true);
            this->addChild(listObjects);
    
            auto objectsPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/";
            float iconHeight = 232;
            float total = iconHeight * soundsName.size() + 125;
            if(total < visibleSize.height - 100){
                total = visibleSize.height - 100;
            }
            listObjects->setInnerContainerSize(Size(visibleSize.width,total));

            for(int i = 0;i < soundsName.size();i++){
                auto fileUtils = FileUtils::getInstance();
                float y = total - (iconHeight * i) - iconHeight/2;
                std::string music;
                if(soundsName[i].length() > 12){
                    music = soundsName[i].substr(0,9); + "...";
                }

                auto name = Label::createWithTTF(music,"fonts/arial.ttf",68 - music.length() * 2);
                name->setPosition(Vec2(origin.x + visibleSize.width/2,y));
                listObjects->addChild(name);

                auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
                settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
                settings->setScale(0.1f);
                listObjects->addChild(settings);
                settings->addClickEventListener([=](Object* sender){
                    frontWindow->setVisible(true);
                    backLayer->setVisible(true);
                    deleteObject->setVisible(true);
                    deleteLabel->setVisible(true);
                    //buttonTextField->setVisible(true);
                    //nameObject->setVisible(true);
                    //objectNameLabel->setVisible(true);
                    //nameObject->setString(soundsName[i]);            
                    DATAmanager::getInstance()->soundName = soundsName[i];
                });

                auto next = ui::Button::create("sprites/button_play.png","sprites/button_play_pressed.png");
                next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                next->setScale(0.1f);
                listObjects->addChild(next);

                auto stop = ui::Button::create("sprites/button_stop.png","sprites/button_stop_pressed.png");
                stop->setVisible(false);
                stop->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                stop->setScale(0.1f);
                listObjects->addChild(stop);

                next->addClickEventListener([=](Object* sender){
                    int checker = DATAmanager::getInstance()->sound_id;
                    if(checker != 0){
                        AudioEngine::stop(checker);
                        if(DATAmanager::getInstance()->activePlayButton && DATAmanager::getInstance()->activeStopButton){
                            DATAmanager::getInstance()->activePlayButton->setVisible(true);
                            DATAmanager::getInstance()->activeStopButton->setVisible(false);
                        }
                    }
                    auto path = DATAmanager::getInstance()->projectPath + "sounds/" + soundsName[i];
                    int sound = AudioEngine::play2d(path,false);
                    next->setVisible(false);
                    stop->setVisible(true);
                    DATAmanager::getInstance()->sound_id = sound;
                    DATAmanager::getInstance()->activePlayButton = next;
                    DATAmanager::getInstance()->activeStopButton = stop;
                });

                stop->addClickEventListener([=](Object* sender){
                    next->setVisible(true);
                    stop->setVisible(false);
                    int sound = DATAmanager::getInstance()->sound_id;
                    AudioEngine::stop(sound);
                    DATAmanager::getInstance()->sound_id = 0;
                    DATAmanager::getInstance()->activePlayButton = nullptr;
                    DATAmanager::getInstance()->activeStopButton = nullptr;
                });

                auto icon = Sprite::create("sprites/icon_sound.png");
                icon->setPosition(Vec2(100,y));
                icon->setScale(0.25f);
                listObjects->addChild(icon);

                auto up = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                up->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y + 50));
                up->setScale(0.1f);
                listObjects->addChild(up);
                up->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "sounds/" + "audio.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == soundsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index > 0){
                        std::swap(sequance[index],sequance[index - 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
                auto down = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                down->setRotation(180);
                down->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y - 50));
                down->setScale(0.1f);
                listObjects->addChild(down);
                down->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "sounds/" + "audio.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
    
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == soundsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index < sequance.size() - 1){
                        std::swap(sequance[index],sequance[index + 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
            }
            this->getScheduler()->schedule([=](float dt){
                int sound = DATAmanager::getInstance()->sound_id;
                if(sound != 0){
                    auto state = AudioEngine::getState(sound);
                    if(state == AudioEngine::AudioState::ERROR){
                        if(DATAmanager::getInstance()->activePlayButton && DATAmanager::getInstance()->activeStopButton){
                            DATAmanager::getInstance()->activePlayButton->setVisible(true);
                            DATAmanager::getInstance()->activeStopButton->setVisible(false);
                        }
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                    } else {
                        DATAmanager::getInstance()->activePlayButton->setVisible(false);
                        DATAmanager::getInstance()->activeStopButton->setVisible(true);
                    }
                }
            },this, 0.2f,false,"global_audio_check");

            break;
        }
        case 2:
        {
            std::vector<std::string> soundsName;
            auto fileUtils = FileUtils::getInstance();
            std::string configSequance = DATAmanager::getInstance()->projectPath + "fonts/fonts.plist";
            if(fileUtils->isFileExist(configSequance)){
                ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                for(const auto& value : sequance){
                    soundsName.push_back(value.asString());
                }  
            }

            Color3B bgColor(255, 100, 100);
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
            Color4F col = Color4F(1,0.14f,0.14f,1.0f);
            frontWindow->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200), col);
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

            auto objectNameLabel = Label::createWithTTF(text[9],titleF,46);    
            objectNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
            objectNameLabel->setVisible(false);
            this->addChild(objectNameLabel,33);
    
            auto buttonTextField = Sprite::create("sprites/textfield.png");
            buttonTextField->setVisible(false);
            buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
            buttonTextField->setScale(0.4f);
            auto textFieldSize = buttonTextField->getContentSize();
            this->addChild(buttonTextField,32);

            auto deleteObject = ui::Button::create("sprites/button_1_1.png","sprites/button_1_1_pressed.png");
            deleteObject->setVisible(false);
            deleteObject->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + 250));
            deleteObject->setScale(0.4f);
            auto textSize = deleteObject->getContentSize();
            this->addChild(deleteObject,39);
            deleteObject->addClickEventListener([=](Object* sender){
                auto fileUtils = FileUtils::getInstance();
                auto soundPlace = DATAmanager::getInstance()->projectPath + "sounds/" + DATAmanager::getInstance()->soundName;
                fileUtils->removeDirectory(soundPlace);

                std::string objectsConfig = DATAmanager::getInstance()->projectPath + "sounds/audio.plist";
                ValueVector sequance = fileUtils->getValueVectorFromFile(objectsConfig);
                for(auto it = sequance.begin();it != sequance.end();++it){
                    if(it->asString() == DATAmanager::getInstance()->soundName){
                        sequance.erase(it);
                        break;
                    }
                }
                fileUtils->writeValueVectorToFile(sequance,objectsConfig);

                auto reload = InsideScene::createScene();        
                Director::getInstance()->replaceScene(reload);
            });

            auto deleteLabel = Label::createWithTTF(text[8],titleB,142);
            deleteLabel->setVisible(false);
            deleteLabel->setTextColor(Color4B::BLACK);
            deleteObject->addChild(deleteLabel);
            deleteLabel->setPosition(Vec2(textSize.width/2,textSize.height/2));

            auto nameObject = ui::TextField::create(text[10],"fonts/arial.ttf",86);    
            nameObject->setVisible(false);
            nameObject->setMaxLengthEnabled(true);
            nameObject->setMaxLength(64);
            nameObject->setTextColor(Color4B::BLACK);
            buttonTextField->addChild(nameObject);
            nameObject->setPosition(Vec2(textFieldSize.width/2,textFieldSize.height/2));

            auto DrawNode = DrawNode::create();    
            Color4F color = Color4F(1,0.14f,0.14f,1.0f);
            DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 150),color);
            this->addChild(DrawNode,20);

            float xDuration = 175;    
            for(int i = 1;i < 5;i++){
                float x = xDuration * i;
                if(i == 1){
                    auto microButton = ui::Button::create("sprites/button_2_0.png","sprites/button_2_0.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 0;
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto objectsIcon = Sprite::create("sprites/icon_objects.png");            
                    objectsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    objectsIcon->setScale(0.08f);
                    this->addChild(objectsIcon,15 + i);
                }
                if(i == 2){
                    auto microButton = ui::Button::create("sprites/button_2_1.png","sprites/button_2_1.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 1;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });
            
                    auto soundsIcon = Sprite::create("sprites/icon_sound.png");
                    soundsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    soundsIcon->setScale(0.08f);
                    this->addChild(soundsIcon,15 + i);
                }
                if(i == 3){
                    auto microButton = ui::Button::create("sprites/button_2_3.png","sprites/button_2_3.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);

                    auto fontsIcon = Sprite::create("sprites/icon_fonts.png");            
                    fontsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    fontsIcon->setScale(0.1f);
                    this->addChild(fontsIcon,15 + i);
                }
                if(i == 4){
                    auto microButton = ui::Button::create("sprites/button_2_2.png","sprites/button_2_2.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 3;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto texturesIcon = Sprite::create("sprites/icon_textures.png");            
                    texturesIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    texturesIcon->setScale(0.08f);
                    this->addChild(texturesIcon,15 + i);
                }
            }
            backLayer->addClickEventListener([=](Object* sender){
        if(!canFadeOut){
            return;
        }
        auto fileUtils = FileUtils::getInstance();
        backLayer->setVisible(false);
        frontWindow->setVisible(false);
        deleteObject->setVisible(false);
        buttonTextField->setVisible(false);
        nameObject->setVisible(false);
        objectNameLabel->setVisible(false);
        auto oldName = DATAmanager::getInstance()->soundName;
        auto newName = std::string(nameObject->getString());
        std::string sequancePath = DATAmanager::getInstance()->projectPath + "sounds/" + "/audio.plist";
        if(oldName == newName){
            return;
        }
        if(newName.empty()){
            return;
        }
        auto oldPath = DATAmanager::getInstance()->projectPath + "sounds/" + oldName;
        auto newPath = DATAmanager::getInstance()->projectPath + "sounds/" + newName;
        fileUtils->renameFile(oldPath,newPath);
        ValueVector renameSequance = fileUtils->getValueVectorFromFile(sequancePath);

        for(int i = 0; i < renameSequance.size(); i++){
        if(renameSequance[i].asString() == oldName){
            renameSequance[i] = Value(newName);
            break;
        }
    }
        fileUtils->writeValueVectorToFile(renameSequance,sequancePath);
        auto reload = InsideScene::createScene();
        Director::getInstance()->replaceScene(reload);
    });

            std::string name = DATAmanager::getInstance()->sceneName;    
            auto nameLabel = Label::createWithTTF(name,"fonts/arial.ttf",86 - name.length() * 2);
            nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 85));
            this->addChild(nameLabel, 21);
    
            auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
            back->setScale(0.2f);
            auto backSize = back->getContentSize();
            back->setPosition(Vec2(origin.x + 50,visibleSize.height - 50));
            back->addClickEventListener([=](Object* sender){
                AudioEngine::stopAll();
                DATAmanager::getInstance()->sound_id = 0;
                DATAmanager::getInstance()->activePlayButton = nullptr;
                DATAmanager::getInstance()->activeStopButton = nullptr;
                auto nextScene = Scenes::createScene();
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
                AudioEngine::stopAll();
                DATAmanager::getInstance()->sound_id = 0;
                DATAmanager::getInstance()->activePlayButton = nullptr;
                DATAmanager::getInstance()->activeStopButton = nullptr;
                openSound();
            });

            auto listObjects = ui::ScrollView::create();    
            listObjects->setContentSize(Size(visibleSize.width,visibleSize.height - 225));
            listObjects->setPosition(Vec2(origin.x,origin.y));
            listObjects->setDirection(ui::ScrollView::Direction::VERTICAL);
            listObjects->setBounceEnabled(true);
            this->addChild(listObjects);
    
            auto objectsPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/";
            float iconHeight = 232;
            float total = iconHeight * soundsName.size() + 125;
            if(total < visibleSize.height - 100){
                total = visibleSize.height - 100;
            }
            listObjects->setInnerContainerSize(Size(visibleSize.width,total));

            for(int i = 0;i < soundsName.size();i++){
                auto fileUtils = FileUtils::getInstance();
                float y = total - (iconHeight * i) - iconHeight/2;
                std::string music;
                if(soundsName[i].length() > 12){
                    music = soundsName[i].substr(0,9); + "...";
                }

                auto name = Label::createWithTTF(music,"fonts/arial.ttf",68 - music.length() * 2);
                name->setPosition(Vec2(origin.x + visibleSize.width/2,y));
                listObjects->addChild(name);

                auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
                settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
                settings->setScale(0.1f);
                //listObjects->addChild(settings);
                settings->addClickEventListener([=](Object* sender){
                    frontWindow->setVisible(true);
                    backLayer->setVisible(true);
                    deleteObject->setVisible(true);
                    deleteLabel->setVisible(true);
                    buttonTextField->setVisible(true);
                    nameObject->setVisible(true);
                    objectNameLabel->setVisible(true);
                    nameObject->setString(soundsName[i]);            
                    DATAmanager::getInstance()->soundName = soundsName[i];
                });

                auto next = ui::Button::create("sprites/button_play.png","sprites/button_play_pressed.png");
                next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                next->setScale(0.1f);
                listObjects->addChild(next);

                auto stop = ui::Button::create("sprites/button_stop.png","sprites/button_stop_pressed.png");
                stop->setVisible(false);
                stop->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                stop->setScale(0.1f);
                listObjects->addChild(stop);

                next->addClickEventListener([=](Object* sender){
                    int checker = DATAmanager::getInstance()->sound_id;
                    if(checker != 0){
                        AudioEngine::stop(checker);
                        if(DATAmanager::getInstance()->activePlayButton && DATAmanager::getInstance()->activeStopButton){
                            DATAmanager::getInstance()->activePlayButton->setVisible(true);
                            DATAmanager::getInstance()->activeStopButton->setVisible(false);
                        }
                    }
                    auto path = DATAmanager::getInstance()->projectPath + "sounds/" + soundsName[i];
                    int sound = AudioEngine::play2d(path,false);
                    next->setVisible(false);
                    stop->setVisible(true);
                    DATAmanager::getInstance()->sound_id = sound;
                    DATAmanager::getInstance()->activePlayButton = next;
                    DATAmanager::getInstance()->activeStopButton = stop;
                });

                stop->addClickEventListener([=](Object* sender){
                    next->setVisible(true);
                    stop->setVisible(false);
                    int sound = DATAmanager::getInstance()->sound_id;
                    AudioEngine::stop(sound);
                    DATAmanager::getInstance()->sound_id = 0;
                    DATAmanager::getInstance()->activePlayButton = nullptr;
                    DATAmanager::getInstance()->activeStopButton = nullptr;
                });

                auto icon = Sprite::create("sprites/icon_sound.png");
                icon->setPosition(Vec2(100,y));
                icon->setScale(0.25f);
                listObjects->addChild(icon);

                auto up = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                up->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y + 50));
                up->setScale(0.1f);
                listObjects->addChild(up);
                up->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "sounds/" + "audio.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == soundsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index > 0){
                        std::swap(sequance[index],sequance[index - 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
                auto down = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                down->setRotation(180);
                down->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y - 50));
                down->setScale(0.1f);
                listObjects->addChild(down);
                down->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "sounds/" + "audio.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
    
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == soundsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index < sequance.size() - 1){
                        std::swap(sequance[index],sequance[index + 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
            }
            
            break;
        }
        case 3:
        {
            std::vector<std::string> soundsName;
            auto fileUtils = FileUtils::getInstance();
            std::string configSequance = DATAmanager::getInstance()->projectPath + "fonts/fonts.plist";
            if(fileUtils->isFileExist(configSequance)){
                ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                for(const auto& value : sequance){
                    soundsName.push_back(value.asString());
                }  
            }

            Color3B bgColor(255, 100, 100);
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
            Color4F col = Color4F(1,0.14f,0.14f,1.0f);
            frontWindow->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200), col);
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

            auto objectNameLabel = Label::createWithTTF(text[9],titleF,46);    
            objectNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
            objectNameLabel->setVisible(false);
            this->addChild(objectNameLabel,33);
    
            auto buttonTextField = Sprite::create("sprites/textfield.png");
            buttonTextField->setVisible(false);
            buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
            buttonTextField->setScale(0.4f);
            auto textFieldSize = buttonTextField->getContentSize();
            this->addChild(buttonTextField,32);

            auto deleteObject = ui::Button::create("sprites/button_1_1.png","sprites/button_1_1_pressed.png");
            deleteObject->setVisible(false);
            deleteObject->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + 250));
            deleteObject->setScale(0.4f);
            auto textSize = deleteObject->getContentSize();
            this->addChild(deleteObject,39);
            deleteObject->addClickEventListener([=](Object* sender){
                auto fileUtils = FileUtils::getInstance();
                auto soundPlace = DATAmanager::getInstance()->projectPath + "sounds/" + DATAmanager::getInstance()->soundName;
                fileUtils->removeDirectory(soundPlace);

                std::string objectsConfig = DATAmanager::getInstance()->projectPath + "sounds/audio.plist";
                ValueVector sequance = fileUtils->getValueVectorFromFile(objectsConfig);
                for(auto it = sequance.begin();it != sequance.end();++it){
                    if(it->asString() == DATAmanager::getInstance()->soundName){
                        sequance.erase(it);
                        break;
                    }
                }
                fileUtils->writeValueVectorToFile(sequance,objectsConfig);

                auto reload = InsideScene::createScene();        
                Director::getInstance()->replaceScene(reload);
            });

            auto deleteLabel = Label::createWithTTF(text[8],titleB,142);
            deleteLabel->setVisible(false);
            deleteLabel->setTextColor(Color4B::BLACK);
            deleteObject->addChild(deleteLabel);
            deleteLabel->setPosition(Vec2(textSize.width/2,textSize.height/2));

            auto nameObject = ui::TextField::create(text[10],"fonts/arial.ttf",86);    
            nameObject->setVisible(false);
            nameObject->setMaxLengthEnabled(true);
            nameObject->setMaxLength(64);
            nameObject->setTextColor(Color4B::BLACK);
            buttonTextField->addChild(nameObject);
            nameObject->setPosition(Vec2(textFieldSize.width/2,textFieldSize.height/2));

            auto DrawNode = DrawNode::create();    
            Color4F color = Color4F(1,0.14f,0.14f,1.0f);
            DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 150),color);
            this->addChild(DrawNode,20);

            float xDuration = 175;    
            for(int i = 1;i < 5;i++){
                float x = xDuration * i;
                if(i == 1){
                    auto microButton = ui::Button::create("sprites/button_2_0.png","sprites/button_2_0.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 0;
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto objectsIcon = Sprite::create("sprites/icon_objects.png");            
                    objectsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    objectsIcon->setScale(0.08f);
                    this->addChild(objectsIcon,15 + i);
                }
                if(i == 2){
                    auto microButton = ui::Button::create("sprites/button_2_1.png","sprites/button_2_1.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 1;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });
            
                    auto soundsIcon = Sprite::create("sprites/icon_sound.png");
                    soundsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    soundsIcon->setScale(0.08f);
                    this->addChild(soundsIcon,15 + i);
                }
                if(i == 3){
                    auto microButton = ui::Button::create("sprites/button_2_3.png","sprites/button_2_3.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);

                    auto fontsIcon = Sprite::create("sprites/icon_fonts.png");            
                    fontsIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    fontsIcon->setScale(0.1f);
                    this->addChild(fontsIcon,15 + i);
                }
                if(i == 4){
                    auto microButton = ui::Button::create("sprites/button_2_2.png","sprites/button_2_2.png");
                    microButton->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    microButton->setScale(0.25f);
                    this->addChild(microButton,14 + i);
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 3;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    });

                    auto texturesIcon = Sprite::create("sprites/icon_textures.png");            
                    texturesIcon->setPosition(Vec2(x - 75,visibleSize.height - 185));
                    texturesIcon->setScale(0.08f);
                    this->addChild(texturesIcon,15 + i);
                }
            }
            backLayer->addClickEventListener([=](Object* sender){
        if(!canFadeOut){
            return;
        }
        auto fileUtils = FileUtils::getInstance();
        backLayer->setVisible(false);
        frontWindow->setVisible(false);
        deleteObject->setVisible(false);
        buttonTextField->setVisible(false);
        nameObject->setVisible(false);
        objectNameLabel->setVisible(false);
        auto oldName = DATAmanager::getInstance()->soundName;
        auto newName = std::string(nameObject->getString());
        std::string sequancePath = DATAmanager::getInstance()->projectPath + "sounds/" + "/audio.plist";
        if(oldName == newName){
            return;
        }
        if(newName.empty()){
            return;
        }
        auto oldPath = DATAmanager::getInstance()->projectPath + "sounds/" + oldName;
        auto newPath = DATAmanager::getInstance()->projectPath + "sounds/" + newName;
        fileUtils->renameFile(oldPath,newPath);
        ValueVector renameSequance = fileUtils->getValueVectorFromFile(sequancePath);

        for(int i = 0; i < renameSequance.size(); i++){
        if(renameSequance[i].asString() == oldName){
            renameSequance[i] = Value(newName);
            break;
        }
    }
        fileUtils->writeValueVectorToFile(renameSequance,sequancePath);
        auto reload = InsideScene::createScene();
        Director::getInstance()->replaceScene(reload);
    });

            std::string name = DATAmanager::getInstance()->sceneName;    
            auto nameLabel = Label::createWithTTF(name,"fonts/arial.ttf",86 - name.length() * 2);
            nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 85));
            this->addChild(nameLabel, 21);
    
            auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
            back->setScale(0.2f);
            auto backSize = back->getContentSize();
            back->setPosition(Vec2(origin.x + 50,visibleSize.height - 50));
            back->addClickEventListener([=](Object* sender){
                AudioEngine::stopAll();
                DATAmanager::getInstance()->sound_id = 0;
                DATAmanager::getInstance()->activePlayButton = nullptr;
                DATAmanager::getInstance()->activeStopButton = nullptr;
                auto nextScene = Scenes::createScene();
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
                AudioEngine::stopAll();
                DATAmanager::getInstance()->sound_id = 0;
                DATAmanager::getInstance()->activePlayButton = nullptr;
                DATAmanager::getInstance()->activeStopButton = nullptr;
                openSound();
            });

            auto listObjects = ui::ScrollView::create();    
            listObjects->setContentSize(Size(visibleSize.width,visibleSize.height - 225));
            listObjects->setPosition(Vec2(origin.x,origin.y));
            listObjects->setDirection(ui::ScrollView::Direction::VERTICAL);
            listObjects->setBounceEnabled(true);
            this->addChild(listObjects);
    
            auto objectsPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/";
            float iconHeight = 232;
            float total = iconHeight * soundsName.size() + 125;
            if(total < visibleSize.height - 100){
                total = visibleSize.height - 100;
            }
            listObjects->setInnerContainerSize(Size(visibleSize.width,total));

            for(int i = 0;i < soundsName.size();i++){
                auto fileUtils = FileUtils::getInstance();
                float y = total - (iconHeight * i) - iconHeight/2;
                std::string music;
                if(soundsName[i].length() > 12){
                    music = soundsName[i].substr(0,9); + "...";
                }

                auto name = Label::createWithTTF(music,"fonts/arial.ttf",68 - music.length() * 2);
                name->setPosition(Vec2(origin.x + visibleSize.width/2,y));
                listObjects->addChild(name);

                auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
                settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
                settings->setScale(0.1f);
                listObjects->addChild(settings);
                settings->addClickEventListener([=](Object* sender){
                    frontWindow->setVisible(true);
                    backLayer->setVisible(true);
                    deleteObject->setVisible(true);
                    deleteLabel->setVisible(true);
                    //buttonTextField->setVisible(true);
                    //nameObject->setVisible(true);
                    //objectNameLabel->setVisible(true);
                    //nameObject->setString(soundsName[i]);            
                    DATAmanager::getInstance()->soundName = soundsName[i];
                });

                auto next = ui::Button::create("sprites/button_play.png","sprites/button_play_pressed.png");
                next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                next->setScale(0.1f);
                listObjects->addChild(next);

                auto stop = ui::Button::create("sprites/button_stop.png","sprites/button_stop_pressed.png");
                stop->setVisible(false);
                stop->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                stop->setScale(0.1f);
                listObjects->addChild(stop);

                next->addClickEventListener([=](Object* sender){
                    int checker = DATAmanager::getInstance()->sound_id;
                    if(checker != 0){
                        AudioEngine::stop(checker);
                        if(DATAmanager::getInstance()->activePlayButton && DATAmanager::getInstance()->activeStopButton){
                            DATAmanager::getInstance()->activePlayButton->setVisible(true);
                            DATAmanager::getInstance()->activeStopButton->setVisible(false);
                        }
                    }
                    auto path = DATAmanager::getInstance()->projectPath + "sounds/" + soundsName[i];
                    int sound = AudioEngine::play2d(path,false);
                    next->setVisible(false);
                    stop->setVisible(true);
                    DATAmanager::getInstance()->sound_id = sound;
                    DATAmanager::getInstance()->activePlayButton = next;
                    DATAmanager::getInstance()->activeStopButton = stop;
                });

                stop->addClickEventListener([=](Object* sender){
                    next->setVisible(true);
                    stop->setVisible(false);
                    int sound = DATAmanager::getInstance()->sound_id;
                    AudioEngine::stop(sound);
                    DATAmanager::getInstance()->sound_id = 0;
                    DATAmanager::getInstance()->activePlayButton = nullptr;
                    DATAmanager::getInstance()->activeStopButton = nullptr;
                });

                auto icon = Sprite::create("sprites/icon_sound.png");
                icon->setPosition(Vec2(100,y));
                icon->setScale(0.25f);
                listObjects->addChild(icon);

                auto up = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                up->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y + 50));
                up->setScale(0.1f);
                listObjects->addChild(up);
                up->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "sounds/" + "audio.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == soundsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index > 0){
                        std::swap(sequance[index],sequance[index - 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
                auto down = ui::Button::create("sprites/sorting.png","sprites/sorting_pressed.png");
                down->setRotation(180);
                down->setPosition(Vec2(origin.x + visibleSize.width/2 - 150,y - 50));
                down->setScale(0.1f);
                listObjects->addChild(down);
                down->addClickEventListener([=](Object* sender){
                    auto fileUtils = FileUtils::getInstance();
                    auto configSequance = DATAmanager::getInstance()->projectPath + "sounds/" + "audio.plist";
                    if(!fileUtils->isFileExist(configSequance)){
                        return;
                    }
                    ValueVector sequance = fileUtils->getValueVectorFromFile(configSequance);
    
                    int index = -1;
                    for(int a = 0;a < sequance.size();a++){
                        if(sequance[a].asString() == soundsName[i]){
                            index = a;
                            break;
                        }
                    }
                    if(index < sequance.size() - 1){
                        std::swap(sequance[index],sequance[index + 1]);
                        fileUtils->writeValueVectorToFile(sequance,configSequance);
                        AudioEngine::stopAll();
                        DATAmanager::getInstance()->sound_id = 0;
                        DATAmanager::getInstance()->activePlayButton = nullptr;
                        DATAmanager::getInstance()->activeStopButton = nullptr;
                        auto reload = InsideScene::createScene();
                        Director::getInstance()->replaceScene(reload);
                    }
                });
            }

            break;
        }
    }

    
scheduleUpdate();

return true;
}
void InsideScene::update(float delta)
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

void InsideScene::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}