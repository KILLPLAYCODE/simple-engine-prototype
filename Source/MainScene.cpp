//MainScene.cpp
#include "MainScene.h"
#include "ui/CocosGUI.h"
#include "CreateProj.h"
#include "ListProject.h"
#include "src/DATAmanager.h"
#include "src/language.h"
#include "axmol.h"
#include <map>
#include <random>
#include <iostream>
#include "Settings.h"
#include "cocostudio/CocosStudioExtension.h"
#define JSON_HAS_CPP_20 0
#include "src/json.hpp"
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
float randomGenerator(float start, float end){
    std::random_device(ran);
    std::mt19937 gen(ran());
    std::uniform_real_distribution<float> dist(start,end);
    float result = dist(gen);
    return result;
}

void runParticle(Node* scene, Vec2 visibleSize, Vec2 origin){
    auto _emitter = ParticleSystemQuad::createWithTotalParticles(150);
    _emitter->retain();

    scene->addChild(_emitter, 2);
    ////_emitter->release();    // win32 :  use this line or remove this line and use autorelease()
    _emitter->setTexture(Director::getInstance()->getTextureCache()->addImage("particles/stars.png"));

    _emitter->setDuration(-1);

    // gravity
    _emitter->setGravity(Vec2::ZERO);

    // angle
    _emitter->setAngle(90);
    _emitter->setAngleVar(360);

    // speed of particles
    _emitter->setSpeed(160);
    _emitter->setSpeedVar(20);

    // radial
    _emitter->setRadialAccel(0);
    _emitter->setRadialAccelVar(50);

    // tangential
    _emitter->setTangentialAccel(30);
    _emitter->setTangentialAccelVar(0);

    // emitter position
    _emitter->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    _emitter->setPosVar(Vec2(randomGenerator(origin.x,visibleSize.width),randomGenerator(origin.y,visibleSize.height)));

    // life of particles
    _emitter->setLife(4);
    _emitter->setLifeVar(2);

    // spin of particles
    _emitter->setStartSpin(0);
    _emitter->setStartSizeVar(0);
    _emitter->setEndSpin(0);
    _emitter->setEndSpinVar(0);

    // color of particles
    Color4F startColor(0.5f, 0.5f, 0.5f, 1.0f);
    _emitter->setStartColor(startColor);

    Color4F startColorVar(0.5f, 0.5f, 0.5f, 1.0f);
    _emitter->setStartColorVar(startColorVar);

    Color4F endColor(0.1f, 0.1f, 0.1f, 0.2f);
    _emitter->setEndColor(endColor);

    Color4F endColorVar(0.1f, 0.1f, 0.1f, 0.2f);
    _emitter->setEndColorVar(endColorVar);

    // size, in pixels
    _emitter->setStartSize(80.0f);
    _emitter->setStartSizeVar(40.0f);
    _emitter->setEndSize(ParticleSystem::START_SIZE_EQUAL_TO_END_SIZE);

    // emits per second
    _emitter->setEmissionRate(_emitter->getTotalParticles() / _emitter->getLife());

    // additive
    _emitter->setBlendAdditive(true);

    scene->schedule([origin,visibleSize,_emitter](float dt){
        _emitter->setPosVar(Vec2(randomGenerator(origin.x,visibleSize.width),randomGenerator(origin.y,visibleSize.height)));
    }, 0.4f,"star_cycle");
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
    std::string night;

    Color3B bgColor(135, 206, 235);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto logoDay = Sprite::create("sprites/LOGO.png");
    logoDay->setPosition(Vec2(visibleSize.width/2,visibleSize.height + 150));
    logoDay->setScale(1.2f);
    this->addChild(logoDay,20);

    auto logoNight = Sprite::create("sprites/night/LOGO.png");
    logoNight->setPosition(Vec2(visibleSize.width/2,visibleSize.height + 150));
    logoNight->setScale(1.2f);
    this->addChild(logoNight,20);

    auto black_layer = LayerColor::create(Color4B(5,0,32,255),visibleSize.width,visibleSize.height);
    black_layer->setPosition(Vec2::ZERO);
    this->addChild(black_layer,0);

    auto move = MoveTo::create(1.5f,Vec2(visibleSize.width/2,visibleSize.height - 300));
    auto move_ease = EaseBounceOut::create(move);
    logoDay->runAction(Sequence::create(move_ease->clone(),nullptr));
    logoNight->runAction(Sequence::create(move_ease->clone(),nullptr));

    auto button = ui::Button::create("sprites/button.png"sv,"sprites/button.png"sv);
    auto buttonNight = ui::Button::create("sprites/night/button_night.png","sprites/night/button_night.png");
    auto button2 = ui::Button::create("sprites/button.png"sv,"sprites/button.png"sv);
    auto buttonNight1 = ui::Button::create("sprites/night/button_night.png","sprites/night/button_night.png");

    auto fileUtils = FileUtils::getInstance();
    std::string languageConfig = fileUtils->getWritablePath() + "language.cf";
    std::string themeConfig = fileUtils->getWritablePath() + "theme.cf";
    if(fileUtils->isFileExist(languageConfig) && fileUtils->isFileExist(themeConfig)){
        DATAmanager::getInstance()->language = fileUtils->getStringFromFile(languageConfig);
        DATAmanager::getInstance()->theme = std::stoi(fileUtils->getStringFromFile(themeConfig));
    } else {
        std::string l = DATAmanager::getInstance()->language;
        fileUtils->writeStringToFile(l,languageConfig);
        std::string t = std::to_string(DATAmanager::getInstance()->theme);
        fileUtils->writeStringToFile(t,themeConfig);
    }

    if(DATAmanager::getInstance()->theme == 0){
        black_layer->setOpacity(0);
        buttonNight->setVisible(false);
        buttonNight1->setVisible(false);
        logoNight->setVisible(false);
        logoDay->setVisible(true);
        logoDay->setOpacity(255);
    }
    if(DATAmanager::getInstance()->theme == 1){
        button->setVisible(false);
        button2->setVisible(false);
        logoDay->setVisible(false);
        logoDay->setOpacity(0);
        button->setOpacity(0);
        button2->setOpacity(0);
        buttonNight->setOpacity(255);
        buttonNight1->setOpacity(255);
        logoNight->setOpacity(255);
        black_layer->setOpacity(255);
        runParticle(this,visibleSize,origin);
    }

    bool read = DATAmanager::getInstance()->LanguageLoad;
    bool readT = DATAmanager::getInstance()->ThemeLoad;
    std::string l = DATAmanager::getInstance()->language;
    if(!read){
        DATAmanager::loadSystemCommands(DATAmanager::getInstance()->commands,DATAmanager::getInstance()->systemCommands);
        std::cout<<DATAmanager::getInstance()->theme<<std::endl;
        LANGUAGE::changeLanguage(l);
        DATAmanager::getInstance()->LanguageLoad = true;
    }
    if(!readT){
        if(DATAmanager::getInstance()->theme == 1){
            black_layer->setOpacity(0);
            buttonNight->setOpacity(0);
            buttonNight1->setOpacity(0);
            logoNight->setOpacity(0);
            logoNight->setVisible(true);
            button->setVisible(true);
            button2->setVisible(true);
            buttonNight->setVisible(true);
            buttonNight1->setVisible(true);
            logoDay->setVisible(true);
            logoDay->setOpacity(255);
            button->setOpacity(255);
            button2->setOpacity(255);
            auto overlay = LayerColor::create(Color4B(5,0,32,0),visibleSize.width,visibleSize.height);
            this->addChild(overlay,1);
            auto fadeIn = FadeIn::create(1);
            auto fadeOut = FadeOut::create(1);
            overlay->runAction(Sequence::create(fadeIn,nullptr,nullptr));
            buttonNight->runAction(FadeIn::create(1));
            button->runAction(FadeOut::create(1));
            buttonNight1->runAction(FadeIn::create(1));
            button2->runAction(FadeOut::create(1));
            logoNight->runAction(FadeIn::create(1.5f));
            logoDay->runAction(FadeOut::create(1.5f));
            runParticle(this,visibleSize,origin);
            DATAmanager::getInstance()->ThemeLoad = true;
        }
    }
    
    text = DATAmanager::getInstance()->translater;
    std::string font = DATAmanager::getInstance()->buttonFont;
    float fontScale = DATAmanager::getInstance()->fontSize;

    //if(DATAmanager::getInstance()->theme != 0){
    //    button->setVisible(false);
    //}
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
    button->setTitleColor(Color3B(20,20,20));
    button->addClickEventListener([this](Object* sender){
        auto nextScene = CreateProj::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    buttonNight->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 + 50));
    buttonNight->setScale(0.6f);
    if(DATAmanager::getInstance()->theme != 1){
        buttonNight->setVisible(false);
    }
    this->addChild(buttonNight,3);
    buttonNight->setTitleText(text[0]);
    buttonNight->setTitleFontName(font);
    buttonNight->setTitleFontSize(fontScale);
    buttonNight->setTitleColor(Color3B(225,225,225));
    buttonNight->addClickEventListener([this](Object* sender){
        auto nextScene = CreateProj::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    //if(DATAmanager::getInstance()->theme != 0){
    //    button2->setVisible(false);
    //}
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
    button2->setTitleColor(Color3B(20,20,20));
    button2->addClickEventListener([this](Object* sender){
        auto nextScene = ListProject::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    buttonNight1->setPosition(Vec2(origin.x + visibleSize.width/2,origin.y + visibleSize.height/2 - 100));
    buttonNight1->setScale(0.6f);
    if(DATAmanager::getInstance()->theme != 1){
        buttonNight1->setVisible(false);
    }
    this->addChild(buttonNight1,3);
    buttonNight1->setTitleText(text[1]);
    buttonNight1->setTitleFontName(font);
    buttonNight1->setTitleFontSize(fontScale - 30);
    buttonNight1->setTitleColor(Color3B(225,225,225));
    buttonNight1->addClickEventListener([this](Object* sender){
        auto nextScene = ListProject::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    auto settings = ui::Button::create("sprites/settings.png","sprites/settings_pressed.png");
    settings->setPosition(Vec2(visibleSize.width - 55,origin.y + 55));
    settings->setScale(0.2f);
    this->addChild(settings,8);
    settings->addClickEventListener([this](Object* sender){
        auto nextScene = Settings::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    auto day_theme = ui::Button::create("sprites/day_theme.png","sprites/day_theme.png");
    day_theme->setPosition(Vec2(origin.x + 60,visibleSize.height - 55));
    day_theme->setScale(0.15f);
    this->addChild(day_theme,18);
    day_theme->addClickEventListener([this](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        std::string themeConfig = fileUtils->getWritablePath() + "theme.cf";
        std::string chose = "1";
        fileUtils->writeStringToFile(chose,themeConfig);
        DATAmanager::getInstance()->ThemeLoad = false;
        DATAmanager::getInstance()->theme = 1;
        auto reload = MainScene::createScene();
        Director::getInstance()->replaceScene(reload);
    });

    auto night_theme = ui::Button::create("sprites/night_theme.png","sprites/night_theme.png");
    night_theme->setPosition(Vec2(origin.x + 60,visibleSize.height - 55));
    night_theme->setScale(0.15f);
    this->addChild(night_theme,22);
    night_theme->addClickEventListener([this](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        std::string themeConfig = fileUtils->getWritablePath() + "theme.cf";
        std::string chose = "0";
        fileUtils->writeStringToFile(chose,themeConfig);
        DATAmanager::getInstance()->theme = 0;
        auto reload = MainScene::createScene();
        Director::getInstance()->replaceScene(reload);
    });

    if(DATAmanager::getInstance()->theme == 0){
        night_theme->setVisible(false);
    } else if(DATAmanager::getInstance()->theme == 1){
        day_theme->setVisible(false);
    }

    std::string systemPath = fileUtils->getWritablePath() + "projects/";
    DATAmanager::getInstance()->projectsFolder = systemPath;

    std::string simpleLibs = fileUtils->getWritablePath() + "libs/";
    std::string defaultLib = simpleLibs + "system/";
    std::string customLib = simpleLibs + "custom/";

    if(!fileUtils->isDirectoryExist(systemPath)){
        fileUtils->createDirectories(systemPath);
    }
    if(!fileUtils->isDirectoryExist(simpleLibs)){
        fileUtils->createDirectories(simpleLibs);
        fileUtils->createDirectories(defaultLib);
        fileUtils->createDirectories(customLib); 
    }

    scheduleUpdate();

    return true;
}

void MainScene::update(float delta)
{
    
}

void MainScene::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}