#include "InsideScene.h"
#include "Scenes.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "MainScene.h"
#include "src/DATAmanager.h"
#include <filesystem>
#include <vector>
#include <map>
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#include "src/portable-file-dialogs.h"
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
static void openGallery(){
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
        std::string spritesPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + DATAmanager::getInstance()->objectName + "/sprite.png";
        Data fileData = fileUtils->getDataFromFile(fullPath);
        if(!fileData.isNull()){
            fileUtils->writeDataToFile(fileData,spritesPath);
            auto reload = InsideScene::createScene();
            Director::getInstance()->replaceScene(reload);
        }
    }
    #endif
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
                fileUtils->createDirectories(systemPath);
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

            std::string name = DATAmanager::getInstance()->sceneName;    
            auto nameLabel = Label::createWithTTF(name,"fonts/arial.ttf",86 - name.length());
            nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 85));
            this->addChild(nameLabel, 21);
    
            auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
            back->setScale(0.2f);
            auto backSize = back->getContentSize();
            back->setPosition(Vec2(origin.x + 75,visibleSize.height - 75));
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
            run->setPosition(Vec2(visibleSize.width - 100,origin.y + 100));
            run->setScale(0.35f);
            this->addChild(run,12);

            auto listObjects = ui::ScrollView::create();    
            listObjects->setContentSize(Size(visibleSize.width,visibleSize.height - 225));
            listObjects->setPosition(Vec2(origin.x,origin.y));
            listObjects->setDirection(ui::ScrollView::Direction::VERTICAL);
            listObjects->setBounceEnabled(true);
            this->addChild(listObjects);

            std::vector<std::string> objectsName;
    
            auto objectsPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/";
            try {
                for(const auto& object : std::filesystem::directory_iterator(objectsPath)){
                    if(object.is_directory()){
                        objectsName.push_back(object.path().filename().string());
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
                AXLOGE("file system error: {}", e.what());
            }
            float iconHeight = 232;
            float total = iconHeight * objectsName.size();
            if(total < visibleSize.height - 225){
                total = visibleSize.height - 225;
            }
            listObjects->setInnerContainerSize(Size(visibleSize.width,total));

            for(int i = 0;i < objectsName.size();i++){
                auto fileUtils = FileUtils::getInstance();
                float y = total - (iconHeight * i) - iconHeight/2;
                std::string spritesPath = DATAmanager::getInstance()->projectPath + "/scenes/" + DATAmanager::getInstance()->sceneName + "/" + objectsName[i] + "/sprite.png";
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

                auto name = Label::createWithTTF(objectsName[i],"fonts/arial.ttf",68 - objectsName[i].length());
                name->setPosition(Vec2(origin.x + visibleSize.width/2,y));
                listObjects->addChild(name);

                auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
                settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
                settings->setScale(0.1f);
                listObjects->addChild(settings);
                settings->addClickEventListener([=](Object* sender){
                    option->setVisible(true);
                    backLayer->setVisible(true);
                    deleteProj->setVisible(true);
                    deleteLabel->setVisible(true);
                    buttonTextField->setVisible(true);
                    nameProject->setVisible(true);
                    projectNameLabel->setVisible(true);
                    nameProject->setString(folderNames[i]);            
                    DATAmanager::getInstance()->objectName = folderNames[i];
                });

                auto next = ui::Button::create("sprites/continue_button.png","sprites/continue_button_pressed.png");
                next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                next->setScale(0.1f);
                listObjects->addChild(next);

                auto icon = Sprite::create("sprites/icon_scene_1.png");
                icon->setPosition(Vec2(100,y));
                icon->setScale(0.25f);
                listObjects->addChild(icon);

            }

            float y = total - iconHeight * objectsName.size() - iconHeight/2;
            auto add = ui::Button::create("sprites/create_add.png","sprites/create_add_pressed.png");
            add->setPosition(Vec2(origin.x + visibleSize.width/2,y));
            add->setScale(0.25f);
            listObjects->addChild(add);
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
            break;
        }
        case 1:
        {
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
            Color4F col = Color4F(0,0.64f,0.9f,1.0f);
            frontWindow->drawSolidRect(Vec2(origin.x + 100,visibleSize.height - 200), Vec2(visibleSize.width - 100,origin.y + 200), col);
            frontWindow->setVisible(false);
            this->addChild(frontWindow,31);

            auto objectNameLabel = Label::createWithTTF(text[5],titleF,46);    
            objectNameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,visibleSize.height - 300));
            objectNameLabel->setVisible(false);
            this->addChild(objectNameLabel,33);
    
            auto buttonTextField = Sprite::create("sprites/textfield.png");
            buttonTextField->setVisible(false);
            buttonTextField->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 200));
            buttonTextField->setScale(0.4f);
            auto textFieldSize = buttonTextField->getContentSize();
            this->addChild(buttonTextField,32);

            auto nameObject = ui::TextField::create(text[6],"fonts/arial.ttf",86);    
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
                auto fileUtils = FileUtils::getInstance();
                auto systemPath = DATAmanager::getInstance()->projectPath + "sounds/";
                if(nameSc.empty()){
                    return;
                }
                if(fileUtils->isDirectoryExist(systemPath)){
                    return;
                }
                fileUtils->createDirectories(systemPath);
                auto nextScene = InsideScene::createScene();
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
                    microButton->addClickEventListener([this](Object* sender){
                        DATAmanager::getInstance()->switcher = 0;
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

            std::string name = DATAmanager::getInstance()->sceneName;    
            auto nameLabel = Label::createWithTTF(name,"fonts/arial.ttf",86 - name.length());
            nameLabel->setPosition(Vec2(origin.x + visibleSize.width/2, visibleSize.height - 85));
            this->addChild(nameLabel, 21);
    
            auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
            back->setScale(0.2f);
            auto backSize = back->getContentSize();
            back->setPosition(Vec2(origin.x + 75,visibleSize.height - 75));
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
            run->setPosition(Vec2(visibleSize.width - 100,origin.y + 100));
            run->setScale(0.35f);
            this->addChild(run,12);

            auto listObjects = ui::ScrollView::create();    
            listObjects->setContentSize(Size(visibleSize.width,visibleSize.height - 225));
            listObjects->setPosition(Vec2(origin.x,origin.y));
            listObjects->setDirection(ui::ScrollView::Direction::VERTICAL);
            listObjects->setBounceEnabled(true);
            this->addChild(listObjects);

            std::vector<std::string> objectsName;
    
            auto objectsPath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/";
            try {
                for(const auto& object : std::filesystem::directory_iterator(objectsPath)){
                    if(object.is_directory()){
                        objectsName.push_back(object.path().filename().string());
                    }
                }
            } catch (const std::filesystem::filesystem_error& e) {
                AXLOGE("file system error: {}", e.what());
            }
            float iconHeight = 232;
            float total = iconHeight * objectsName.size();
            if(total < visibleSize.height - 225){
                total = visibleSize.height - 225;
            }
            listObjects->setInnerContainerSize(Size(visibleSize.width,total));

            for(int i = 0;i < objectsName.size();i++){
                float y = total - (iconHeight * i) - iconHeight/2;
                /*auto picture = Sprite::create("sprites/scene.png");
                picture->setPosition(Vec2(100,y));
                picture->setScale(0.15f);
                listObjects->addChild(picture);
                */

                auto name = Label::createWithTTF(objectsName[i],"fonts/arial.ttf",68 - objectsName[i].length());
                name->setPosition(Vec2(origin.x + visibleSize.width/2,y));
                listObjects->addChild(name);

                auto settings = ui::Button::create("sprites/settings_button.png","sprites/settings_button.png");
                settings->setPosition(Vec2(origin.x + visibleSize.width/2 + 180,y));
                settings->setScale(0.1f);
                listObjects->addChild(settings);

                auto next = ui::Button::create("sprites/continue_button.png","sprites/continue_button_pressed.png");
                next->setPosition(Vec2(origin.x + visibleSize.width/2 + 260,y));
                next->setScale(0.1f);
                listObjects->addChild(next);

                auto icon = Sprite::create("sprites/icon_scene_1.png");
                icon->setPosition(Vec2(100,y));
                icon->setScale(0.25f);
                listObjects->addChild(icon);

            }

            float y = total - iconHeight * objectsName.size() - iconHeight/2;
            auto add = ui::Button::create("sprites/create_add.png","sprites/create_add_pressed.png");
            add->setPosition(Vec2(origin.x + visibleSize.width/2,y));
            add->setScale(0.25f);
            listObjects->addChild(add);
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
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
extern "C" {
    JNIEXPORT void JNICALL Java_dev_prototype_klengine_AppActivity_onImageSelectedNative(JNIEnv* env, jobject thiz, jstring path) {
        std::string cacheImagePath = JniHelper::jstring2string(path);
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([cacheImagePath](){
            auto fileUtils = FileUtils::getInstance();
            std::string filePath = DATAmanager::getInstance()->projectPath + DATAmanager::getInstance->sceneName + "/" + DATAmanager::getInstance->objectName + "/sprite.png";
            Data fileData = fileUtils->getDataFromFile(cacheImagePath);

            if(!fileData.isNull()){
                fileUtils->writeDataToFile(fileData,filePath);
                fileUtils->removeFile(cacheImagePath);
                Director::getInstance()->getTextureCache()->removeTextureForKey(filePath);
                auto scene = InsideScene::createScene();
                Director::getInstance()->replaceScene(scene);
            }
        });
    }
}
#endif