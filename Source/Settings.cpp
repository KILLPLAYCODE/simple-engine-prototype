#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "MainScene.h"
#include "src/DATAmanager.h"
#include "Settings.h"
#include <map>
#include "src/language.h"
#include <iostream>
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
    std::string ButtonFont = DATAmanager::getInstance()->buttonFont;
    int theme = DATAmanager::getInstance()->theme;
    std::vector<int> settingVector = DATAmanager::getInstance()->settingStorage;
    std::string night;
    if(theme == 0){
        night = "";
    }
    if(theme == 1){
        night = "night/";
    }

    vsyncBanner = ui::Layout::create();
    vsyncBanner->setVisible(false);
    vsyncBanner->setPosition(Vec2(origin.x + 100,origin.y + 200));
    vsyncBanner->setContentSize(Size(visibleSize.width - 200,visibleSize.height - 400));
    vsyncBanner->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
    if(theme == 0){
        vsyncBanner->setBackGroundColor(Color3B(0, 91, 218));
    }
    if(theme == 1){
        vsyncBanner->setBackGroundColor(Color3B(5,0,32));
    }
    vsyncBanner->setTouchEnabled(true);
    this->addChild(vsyncBanner,100);

    vsyncBehind = LayerColor::create(Color4B::BLACK,visibleSize.width,visibleSize.height);
    vsyncBehind->setVisible(false);
    vsyncBehind->setPosition(Vec2::ZERO);
    vsyncBehind->setOpacity(125);
    this->addChild(vsyncBehind,99);

    Attention = Label::createWithTTF(text[31],LabelFont,98);
    Attention->setVisible(false);
    Attention->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2 + 400));
    Attention->setTextColor(Color4B::YELLOW);
    this->addChild(Attention,101);

    attention = Label::createWithTTF(text[32],LabelFont,50);
    attention->setVisible(false);
    attention->setPosition(Vec2(visibleSize.width/2,visibleSize.height/2 + 100));
    this->addChild(attention,102);

    confirm = ui::Button::create("sprites/" + night + "button.png","sprites/" + night + "button_pressed.png");
    confirm->setVisible(false);
    confirm->setPosition(Vec2(visibleSize.width/2 + 125,visibleSize.height/2 - 450));
    confirm->setScale(0.3f);
    this->addChild(confirm,103);

    confirm->addClickEventListener([this](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        std::string pathToSettings = fileUtils->getWritablePath() + "settings.plist";
        ValueVector sm;
        if(DATAmanager::getInstance()->VSync){
            sm.push_back(Value(0));
        } else {
            sm.push_back(Value(1));
        }
        sm.push_back(Value(DATAmanager::getInstance()->fps));
        std::cout<<DATAmanager::getInstance()->fps<<std::endl;
        if(DATAmanager::getInstance()->stats){
            sm.push_back(Value(1));
        } else {
            sm.push_back(Value(0));
        }
        fileUtils->writeValueVectorToFile(sm,pathToSettings);
        _director->end();
    });

    auto confLabel = Label::createWithTTF(text[33],font,108);
    confLabel->setPosition(Vec2(confirm->getContentSize().width/2,confirm->getContentSize().height/2));
    if(theme == 0){
        confLabel->setTextColor(Color4B::BLACK);
    }
    if(theme == 1){
        confLabel->setTextColor(Color4B::WHITE);
    }
    confirm->addChild(confLabel);

    cancel = ui::Button::create("sprites/" + night + "button_1_1.png","sprites/" + night + "button_1_1_pressed.png");
    cancel->setVisible(false);
    cancel->setPosition(Vec2(visibleSize.width/2 - 125,visibleSize.height/2 - 450));
    cancel->setScale(0.3f);
    this->addChild(cancel,104);

    cancel->addClickEventListener([this](Object* sender){
        vsyncBanner->setVisible(false);
        vsyncBehind->setVisible(false);
        Attention->setVisible(false);
        attention->setVisible(false);
        confirm->setVisible(false);
        cancel->setVisible(false);
    });

    auto cancelLabel = Label::createWithTTF(text[7],font,108);
    cancelLabel->setPosition(Vec2(confirm->getContentSize().width/2,confirm->getContentSize().height/2));
    if(theme == 0){
        cancelLabel->setTextColor(Color4B::BLACK);
    }
    if(theme == 1){
        cancelLabel->setTextColor(Color4B::WHITE);
    }
    cancel->addChild(cancelLabel);

    touchListener->onTouchBegan = [this,visibleSize](Touch* touch, Event* event)-> bool {
        Vec2 localPoint = vsyncBehind->convertToNodeSpace(touch->getLocation());
        Rect rect = Rect(0,0,visibleSize.width,visibleSize.height);
        if(rect.containsPoint(localPoint)){
            vsyncBanner->setVisible(false);
            vsyncBehind->setVisible(false);
            Attention->setVisible(false);
            attention->setVisible(false);
            confirm->setVisible(false);
            cancel->setVisible(false);
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,vsyncBehind);

    SpriteFrameCache::getInstance()->removeSpriteFrames();
    if(theme == 0){
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprites/slider.plist");
    }

    if(theme == 1){
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("sprites/night/slider.plist");
    }

    Vector<SpriteFrame*> sliderIdle;
    Vector<SpriteFrame*> sliderIdle1;

    sliderIdle.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("frame0001.png"));

    sliderIdle1.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("frame0009.png"));

    Vector<SpriteFrame*> sliderMove;

    for(int i = 0;i < 10;i++){
        std::string number = "frame000" + std::to_string(i) + ".png";
        sliderMove.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(number));
    }

    Vector<SpriteFrame*> sliderMoveReverse;

    for(int i = 9;i > 0;i--){
        std::string number = "frame000" + std::to_string(i) + ".png";
        sliderMoveReverse.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName(number));
    }

    idleAnim = Animation::createWithSpriteFrames(sliderIdle,0.1f);
    idleAnim->retain();

    idleAnim1 = Animation::createWithSpriteFrames(sliderIdle1,0.1f);
    idleAnim1->retain();

    moveAnimRight = Animation::createWithSpriteFrames(sliderMove,0.03f);
    moveAnimRight->retain();

    moveAnimLeft =  Animation::createWithSpriteFrames(sliderMoveReverse,0.03f);
    moveAnimLeft->retain();

    if(theme == 0){
        Color3B bgColor(135, 206, 235);
        auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
        layer->setPosition(Vec2::ZERO);
        this->addChild(layer,0);
    }
    if(theme == 1){
        auto layer = LayerColor::create(Color4B(5,0,32,255),visibleSize.width,visibleSize.height);
        layer->setPosition(Vec2::ZERO);
        this->addChild(layer,0);
    }

    auto list = ui::ScrollView::create();
    list->setInnerContainerSize(Vec2(visibleSize.width,visibleSize.height*2));
    list->setContentSize(Vec2(visibleSize.width + 15,visibleSize.height));
    list->setPosition(Vec2(origin.x,origin.y));
    list->setDirection(ui::ScrollView::Direction::VERTICAL);
    list->setBounceEnabled(true);
    this->addChild(list);

    auto slider = Sprite::createWithSpriteFrame(sliderIdle.front());
    slider->setPosition(Vec2(visibleSize.width - 150,list->getInnerContainerSize().height - 600));
    slider->setScale(0.3f);
    list->addChild(slider);
    if(settingVector[0] == 0){
        slider->runAction(Animate::create(idleAnim));
    }
    if(settingVector[0] == 1){
        slider->runAction(Animate::create(idleAnim1));
    }

    auto slider1 = Sprite::createWithSpriteFrame(sliderIdle.front());
    slider1->setPosition(Vec2(visibleSize.width - 150,list->getInnerContainerSize().height - 800));
    slider1->setScale(0.3f);
    list->addChild(slider1);
    if(settingVector[2] == 0){
        slider1->runAction(Animate::create(idleAnim));
    }
    if(settingVector[2] == 1){
        slider1->runAction(Animate::create(idleAnim1));
    }

    auto sliderButton = ui::Button::create("sprites/" + night + "slider_button.png","sprites/" + night + "slider_button.png");
    if(settingVector[0] == 1){
        sliderButton->setPosition(Vec2(slider->getContentSize().width - 125,slider->getContentSize().height/2));
    }
    if(settingVector[0] == 0){
        sliderButton->setPosition(Vec2(125,slider->getContentSize().height/2));
    }
    sliderButton->setScale(0.98f);
    slider->addChild(sliderButton);

    auto sliderButton1 = ui::Button::create("sprites/" + night + "slider_button.png","sprites/" + night + "slider_button.png");
    if(settingVector[2] == 1){
        sliderButton1->setPosition(Vec2(slider1->getContentSize().width - 125,slider1->getContentSize().height/2));
    }
    if(settingVector[2] == 0){
        sliderButton1->setPosition(Vec2(125,slider1->getContentSize().height/2));
    }
    sliderButton1->setScale(0.98f);
    slider1->addChild(sliderButton1);

    switcher = settingVector[0];
    switcher1 = settingVector[2];

    sliderButton->addClickEventListener([this,slider,sliderButton](Object* sender){
        if(switcher == 1){
            if(!confirmB){
                vsyncBanner->setVisible(true);
                vsyncBehind->setVisible(true);
                Attention->setVisible(true);
                attention->setVisible(true);
                confirm->setVisible(true);
                cancel->setVisible(true);
                return;
            }
            std::cout<<"RUN#1\n";
            switcher = 3;
            auto move = MoveTo::create(0.3f, Vec2(125,slider->getContentSize().height/2));
            auto seq = Sequence::create(Animate::create(moveAnimLeft),CallFunc::create([this]() { switcher = 0;}),nullptr);
            slider->runAction(seq);
            sliderButton->runAction(move);
            return;
        }
        if(switcher == 0){
            if(!confirmB){
                vsyncBanner->setVisible(true);
                vsyncBehind->setVisible(true);
                Attention->setVisible(true);
                attention->setVisible(true);
                confirm->setVisible(true);
                cancel->setVisible(true);
                return;
            }
            std::cout<<"RUN#2\n";
            switcher = 2;
            auto move = MoveTo::create(0.3f, Vec2(slider->getContentSize().width - 125,slider->getContentSize().height/2));
            auto seq = Sequence::create(Animate::create(moveAnimRight),CallFunc::create([this]() { switcher = 1;}),nullptr);
            slider->runAction(seq);
            sliderButton->runAction(move);
            return;
        }
    });

    sliderButton1->addClickEventListener([this,slider1,sliderButton1](Object* sender){
        if(switcher1 == 0){
            Director::getInstance()->setStatsDisplay(true);
            std::cout<<"RUN#3\n";
            switcher1 = 2;
            auto move = MoveTo::create(0.3f, Vec2(slider1->getContentSize().width - 125,slider1->getContentSize().height/2));
            auto seq = Sequence::create(Animate::create(moveAnimRight),CallFunc::create([this]() { switcher1 = 1;}),nullptr);
            slider1->runAction(seq);
            sliderButton1->runAction(move);
            DATAmanager::getInstance()->stats = true;

            auto fileUtils = FileUtils::getInstance();
            std::string pathToPlist = fileUtils->getWritablePath() + "settings.plist";
            DATAmanager::getInstance()->settingStorage.erase(DATAmanager::getInstance()->settingStorage.begin() + 2);
            DATAmanager::getInstance()->settingStorage.push_back(1);
            int index = DATAmanager::getInstance()->settingStorage.size() - 1;
            std::swap(DATAmanager::getInstance()->settingStorage[index],DATAmanager::getInstance()->settingStorage[2]);
            ValueVector settingSave;
            for(const auto& value : DATAmanager::getInstance()->settingStorage){
                settingSave.push_back(Value(value));
            }
            fileUtils->writeValueVectorToFile(settingSave,pathToPlist);

            return;
        }
        if(switcher1 == 1){
            Director::getInstance()->setStatsDisplay(false);
            std::cout<<"RUN#4\n";
            switcher1 = 3;
            auto move = MoveTo::create(0.3f, Vec2(125,slider1->getContentSize().height/2));
            auto seq = Sequence::create(Animate::create(moveAnimLeft),CallFunc::create([this]() { switcher1 = 0;}),nullptr);
            slider1->runAction(seq);
            sliderButton1->runAction(move);
            DATAmanager::getInstance()->stats = false;
            
            auto fileUtils = FileUtils::getInstance();
            std::string pathToPlist = fileUtils->getWritablePath() + "settings.plist";
            DATAmanager::getInstance()->settingStorage.erase(DATAmanager::getInstance()->settingStorage.begin() + 2);
            DATAmanager::getInstance()->settingStorage.push_back(0);
            int index = DATAmanager::getInstance()->settingStorage.size() - 1;
            std::swap(DATAmanager::getInstance()->settingStorage[index],DATAmanager::getInstance()->settingStorage[2]);
            ValueVector settingSave;
            for(const auto& value : DATAmanager::getInstance()->settingStorage){
                settingSave.push_back(Value(value));
            }
            fileUtils->writeValueVectorToFile(settingSave,pathToPlist);

            return;
        }
    });

    auto vsync = Label::createWithTTF("VSync:","fonts/Gokart Bubble.otf",98);
    vsync->setPosition(Vec2(origin.x + 200,list->getInnerContainerSize().height - 600));
    list->addChild(vsync);
///////////////////////////////////////////////////////////////////////////////////////////////////////
    if(DATAmanager::getInstance()->language == "ru"){
        SETTINGS = Label::createWithTTF(text[36],LabelFont,68);
        SETTINGS->setPosition(Vec2(visibleSize.width/2,list->getInnerContainerSize().height - 200));
        list->addChild(SETTINGS,21);
    }
    if(DATAmanager::getInstance()->language == "en"){
        SETTINGS = Label::createWithTTF(text[36],LabelFont,88);
        SETTINGS->setPosition(Vec2(visibleSize.width/2,list->getInnerContainerSize().height - 200));
        list->addChild(SETTINGS,21);
    }
    if(DATAmanager::getInstance()->language == "ukr"){
        SETTINGS = Label::createWithTTF(text[36],LabelFont,58);
        SETTINGS->setPosition(Vec2(visibleSize.width/2,list->getInnerContainerSize().height - 200));
        list->addChild(SETTINGS,21);
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////
    if(DATAmanager::getInstance()->language == "ru"){
        mainSettings = Label::createWithTTF(text[34],LabelFont,68);
        mainSettings->setPosition(Vec2(visibleSize.width/2,list->getInnerContainerSize().height - 400));
        list->addChild(mainSettings,21);
    }
    if(DATAmanager::getInstance()->language == "en"){
        mainSettings = Label::createWithTTF(text[34],LabelFont,88);
        mainSettings->setPosition(Vec2(visibleSize.width/2,list->getInnerContainerSize().height - 400));
        list->addChild(mainSettings,21);
    }
    if(DATAmanager::getInstance()->language == "ukr"){
        mainSettings = Label::createWithTTF(text[34],LabelFont,58);
        mainSettings->setPosition(Vec2(visibleSize.width/2,list->getInnerContainerSize().height - 400));
        list->addChild(mainSettings,21);
    }
    auto tier = DrawNode::create();
    tier->drawSegment(Vec2(origin.x + 100,list->getInnerContainerSize().height - 250),Vec2(visibleSize.width - 100,list->getInnerContainerSize().height - 250),16,Color4F(1,1,1,1));
    list->addChild(tier);
///////////////////////////////////////////////////////////////////////////////////////////////////////
    auto statsLabel = Label::createWithTTF(text[35],font,58);
    statsLabel->setPosition(Vec2(origin.x + 225,list->getInnerContainerSize().height - 800));
    list->addChild(statsLabel);

    auto fpsLabel = Label::createWithTTF("FPS:","fonts/Gokart Bubble.otf",98);
    fpsLabel->setPosition(Vec2(origin.x + 175,list->getInnerContainerSize().height - 1000));
    list->addChild(fpsLabel);

    auto fpsCounter = Label::createWithTTF(std::to_string(DATAmanager::getInstance()->settingStorage[1]),"fonts/Gokart Bubble.otf",108 - std::to_string(DATAmanager::getInstance()->settingStorage[1]).length() * 4);
    fpsCounter->setPosition(Vec2(visibleSize.width - 190,list->getInnerContainerSize().height - 1000));
    list->addChild(fpsCounter);

    auto fpsRight = ui::Button::create("sprites/" + night + "button_play.png","sprites/" + night + "button_play_pressed.png");
    fpsRight->setPosition(Vec2(visibleSize.width - 50,list->getInnerContainerSize().height - 1000));
    fpsRight->setScale(0.1f);
    list->addChild(fpsRight);

    fpsRight->addTouchEventListener([this,fpsCounter](Object* sender,ui::Widget::TouchEventType type){
        switch(type){
            case ui::Widget::TouchEventType::BEGAN:
            {
                DATAmanager::getInstance()->fps++;
                fpsCounter->setString(std::to_string(DATAmanager::getInstance()->fps));

                this->scheduleOnce([this,fpsCounter](float dt){
                    this->schedule([this,fpsCounter](float dt){
                        DATAmanager::getInstance()->fps++;
                        fpsCounter->setString(std::to_string(DATAmanager::getInstance()->fps));
                        TTFConfig autoSize("fonts/Gokart Bubble.otf",108 - fpsCounter->getString().length() * 4);
                        fpsCounter->setTTFConfig(autoSize);
                    }, 0.08f,"fps_+_cycle");
                },0.4f,"fps_+");
                break;
            }
            case ui::Widget::TouchEventType::CANCELED:
            case ui::Widget::TouchEventType::ENDED:
            {
                this->unschedule("fps_+");
                this->unschedule("fps_+_cycle");
                break;
            }
        }
    });

    auto fpsLeft = ui::Button::create("sprites/" + night + "button_play.png","sprites/" + night + "button_play_pressed.png");
    fpsLeft->setPosition(Vec2(visibleSize.width - 325,list->getInnerContainerSize().height - 1000));
    fpsLeft->setScale(0.1f);
    fpsLeft->setScaleX(-fpsLeft->getScaleX());
    list->addChild(fpsLeft);

    fpsLeft->addTouchEventListener([this,fpsCounter](Object* sender,ui::Widget::TouchEventType type){
        switch(type){
            case ui::Widget::TouchEventType::BEGAN:
            {
                DATAmanager::getInstance()->fps--;
                fpsCounter->setString(std::to_string(DATAmanager::getInstance()->fps));

                this->scheduleOnce([this,fpsCounter](float dt){
                    this->schedule([this,fpsCounter](float dt){
                        DATAmanager::getInstance()->fps--;
                        fpsCounter->setString(std::to_string(DATAmanager::getInstance()->fps));
                        TTFConfig autoSize("fonts/Gokart Bubble.otf",108 - fpsCounter->getString().length() * 4);
                        fpsCounter->setTTFConfig(autoSize);
                    }, 0.08f,"fps_-_cycle");
                },0.4f,"fps_-");
                break;
            }
            case ui::Widget::TouchEventType::CANCELED:
            case ui::Widget::TouchEventType::ENDED:
            {
                this->unschedule("fps_-");
                this->unschedule("fps_-_cycle");
                break;
            }
        }
    });

    labelanguage = Label::createWithTTF(text[4],LabelFont,98);
    labelanguage->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + 450));
    list->addChild(labelanguage, 20);

    auto back = ui::Button::create("sprites/" + night + "button_back.png","sprites/" + night + "button_back_pressed.png");
    back->setScale(0.25f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + 75,visibleSize.height - 75));
    back->addClickEventListener([this](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        std::string languageConfig = fileUtils->getWritablePath() + "language.cf";
        std::string settingConf = fileUtils->getWritablePath() + "settings.plist";
        fileUtils->writeStringToFile(DATAmanager::getInstance()->language,languageConfig);
        DATAmanager::getInstance()->settingStorage.erase(DATAmanager::getInstance()->settingStorage.begin() + 1);
        DATAmanager::getInstance()->settingStorage.push_back(DATAmanager::getInstance()->fps);
        int index = DATAmanager::getInstance()->settingStorage.size() - 1;
        std::swap(DATAmanager::getInstance()->settingStorage[index],DATAmanager::getInstance()->settingStorage[1]);
        ValueVector settingVec;
        for(const auto& value : DATAmanager::getInstance()->settingStorage){
            settingVec.push_back(Value(value));
        }
        fileUtils->writeValueVectorToFile(settingVec,settingConf);
        Director::getInstance()->setAnimationInterval(1.0f / (float)DATAmanager::getInstance()->fps);

        auto nextScene = MainScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 25);

    if(theme == 0){
        auto languageButton = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
        languageButton->setPosition(Vec2(origin.x + visibleSize.width/2 - 175,origin.y + 250));
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
            std::string font = DATAmanager::getInstance()->buttonFont;
            TTFConfig config(LabelFont, 98);
            TTFConfig mainS(LabelFont, 88);
            TTFConfig statsTTF(font, 58);
            TTFConfig confCancel(font,108);
            labelanguage->setString(text[4]);
            Attention->setString(text[31]);
            attention->setString(text[32]);
            mainSettings->setString(text[34]);
            statsLabel->setString(text[35]);
            SETTINGS->setString(text[36]);
            confLabel->setString(text[33]);
            cancelLabel->setString(text[7]);
            labelanguage->setTTFConfig(config);
            mainSettings->setTTFConfig(mainS);
            SETTINGS->setTTFConfig(mainS);
            statsLabel->setTTFConfig(statsTTF);   
            confLabel->setTTFConfig(confCancel);
            cancelLabel->setTTFConfig(confCancel); 
        });

        auto languageButton2 = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
        languageButton2->setPosition(Vec2(origin.x + visibleSize.width/2 + 175,origin.y + 250));
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
            std::string font = DATAmanager::getInstance()->buttonFont;
            TTFConfig config(LabelFont, 98);
            TTFConfig mainS(LabelFont, 68);
            TTFConfig statsTTF(font, 58);
            TTFConfig confCancel(font,108);
            labelanguage->setString(text[4]);
            Attention->setString(text[31]);
            attention->setString(text[32]);
            mainSettings->setString(text[34]);
            statsLabel->setString(text[35]);
            SETTINGS->setString(text[36]);
            confLabel->setString(text[33]);
            cancelLabel->setString(text[7]);
            labelanguage->setTTFConfig(config);
            mainSettings->setTTFConfig(mainS);
            SETTINGS->setTTFConfig(mainS);
            statsLabel->setTTFConfig(statsTTF);
            confLabel->setTTFConfig(confCancel);
            cancelLabel->setTTFConfig(confCancel); 
        });

        auto languageButton3 = ui::Button::create("sprites/button_1.png", "sprites/button_1_pressed.png");
        languageButton3->setPosition(Vec2(origin.x + visibleSize.width/2 - 175,origin.y + 100));
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
            std::string font = DATAmanager::getInstance()->buttonFont;
            TTFConfig config(LabelFont, 98);
            TTFConfig mainS(LabelFont, 58);
            TTFConfig statsTTF(font, 58);
            TTFConfig confCancel(font,108);
            labelanguage->setString(text[4]);
            Attention->setString(text[31]);
            attention->setString(text[32]);
            mainSettings->setString(text[34]);
            statsLabel->setString(text[35]);
            SETTINGS->setString(text[36]);
            confLabel->setString(text[33]);
            cancelLabel->setString(text[7]);
            labelanguage->setTTFConfig(config);
            mainSettings->setTTFConfig(mainS);
            SETTINGS->setTTFConfig(mainS);
            statsLabel->setTTFConfig(statsTTF);
            confLabel->setTTFConfig(confCancel);
            cancelLabel->setTTFConfig(confCancel);
        });
    }

    if(theme == 1){
        auto languageButton = ui::Button::create("sprites/night/button_1.png", "sprites/night/button_1_pressed.png");
        languageButton->setPosition(Vec2(origin.x + visibleSize.width/2 - 175,origin.y + 250));
        languageButton->setScale(0.5f);
        list->addChild(languageButton,10);
        languageButton->setTitleText("ENGLISH");
        languageButton->setTitleFontName("fonts/Gokart Bubble.otf");
        languageButton->setTitleFontSize(128);
        languageButton->addClickEventListener([=](Object* sender){
            std::string l = "en";
            LANGUAGE::changeLanguage(l);
            std::string LabelFont = DATAmanager::getInstance()->titleFont;
            std::map<int,std::string> text = DATAmanager::getInstance()->translater;
            std::string font = DATAmanager::getInstance()->buttonFont;
            TTFConfig config(LabelFont, 98);
            TTFConfig mainS(LabelFont, 88);
            TTFConfig statsTTF(font, 58);
            TTFConfig confCancel(font,108);
            labelanguage->setString(text[4]);
            Attention->setString(text[31]);
            attention->setString(text[32]);
            mainSettings->setString(text[34]);
            statsLabel->setString(text[35]);
            SETTINGS->setString(text[36]);
            confLabel->setString(text[33]);
            cancelLabel->setString(text[7]);
            labelanguage->setTTFConfig(config);
            mainSettings->setTTFConfig(mainS);
            SETTINGS->setTTFConfig(mainS);
            statsLabel->setTTFConfig(statsTTF);   
            confLabel->setTTFConfig(confCancel);
            cancelLabel->setTTFConfig(confCancel);     
        });

        auto languageButton2 = ui::Button::create("sprites/night/button_1.png", "sprites/night/button_1_pressed.png");
        languageButton2->setPosition(Vec2(origin.x + visibleSize.width/2 + 175,origin.y + 250));
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
            std::string font = DATAmanager::getInstance()->buttonFont;
            TTFConfig config(LabelFont, 98);
            TTFConfig mainS(LabelFont, 68);
            TTFConfig statsTTF(font, 58);
            TTFConfig confCancel(font,108);
            labelanguage->setString(text[4]);
            Attention->setString(text[31]);
            attention->setString(text[32]);
            mainSettings->setString(text[34]);
            statsLabel->setString(text[35]);
            SETTINGS->setString(text[36]);
            confLabel->setString(text[33]);
            cancelLabel->setString(text[7]);
            labelanguage->setTTFConfig(config);
            mainSettings->setTTFConfig(mainS);
            SETTINGS->setTTFConfig(mainS);
            statsLabel->setTTFConfig(statsTTF);
            confLabel->setTTFConfig(confCancel);
            cancelLabel->setTTFConfig(confCancel);
        });

        auto languageButton3 = ui::Button::create("sprites/night/button_1.png", "sprites/night/button_1_pressed.png");
        languageButton3->setPosition(Vec2(origin.x + visibleSize.width/2 - 175,origin.y + 100));
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
            std::string font = DATAmanager::getInstance()->buttonFont;
            TTFConfig config(LabelFont, 98);
            TTFConfig mainS(LabelFont, 58);
            TTFConfig statsTTF(font, 58);
            TTFConfig confCancel(font,108);
            labelanguage->setString(text[4]);
            Attention->setString(text[31]);
            attention->setString(text[32]);
            mainSettings->setString(text[34]);
            statsLabel->setString(text[35]);
            SETTINGS->setString(text[36]);
            confLabel->setString(text[33]);
            cancelLabel->setString(text[7]);
            labelanguage->setTTFConfig(config);
            mainSettings->setTTFConfig(mainS);
            SETTINGS->setTTFConfig(mainS);
            statsLabel->setTTFConfig(statsTTF);
            confLabel->setTTFConfig(confCancel);
            cancelLabel->setTTFConfig(confCancel);
        });
    }

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
        break;
    }

    }  // switch
}
