#include "CreateProj.h"
#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "Scenes.h"
#include "MainScene.h"
#include "src/DATAmanager.h"
using namespace ax;

Scene* CreateProj::createScene() 
{
    return CreateProj::create();
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
    int theme = DATAmanager::getInstance()->theme;
    
    if(theme == 0){
        Color3B bgColor(135, 206, 235);
        auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
        layer->setPosition(Vec2::ZERO);
        this->addChild(layer,0);
    }
    if(theme == 1){
        Color3B bgColor(5,0,32);
        auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
        layer->setPosition(Vec2::ZERO);
        this->addChild(layer,0);
    }

    if(theme == 0){
        back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    }
    if(theme == 1){
        back = ui::Button::create("sprites/night/button_back.png"sv,"sprites/night/button_back_pressed.png"sv);
    }

    back->setScale(0.25f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + 75,visibleSize.height - 75));
    back->addClickEventListener([=](Object* sender){
        auto nextScene = MainScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 15);

    if(theme == 0){
        sprite = Sprite::create("sprites/textfield.png"sv);
    }
    if(theme == 1){
        sprite = Sprite::create("sprites/night/textfield.png"sv);
    }
    
    sprite->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 + 200));
    sprite->setScale(0.6f);
    Size spritesz = sprite->getContentSize();
    this->addChild(sprite,5);

    if(theme == 0){
        button2 = ui::Button::create("sprites/button.png"sv,"sprites/button_pressed.png"sv);
    }
    if(theme == 1){
        button2 = ui::Button::create("sprites/night/button_night.png"sv,"sprites/night/button_night_pressed.png"sv);
    }
    
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
    if(theme == 0){
        button2->setTitleColor(Color3B::BLACK);
    }

    auto textfield = ui::TextField::create(text[3], "fonts/arial.ttf",86);
    textfield->setPosition(Vec2(spritesz.width/2,spritesz.height/2));
    textfield->setMaxLengthEnabled(true);
    textfield->setMaxLength(18);
    if(theme == 0){
        textfield->setTextColor(Color4B::BLACK);
    }
    
    sprite->addChild(textfield,8);

    button2->addClickEventListener([=](Object* sender){
        auto fileUtils = FileUtils::getInstance();
        std::string systemPath = fileUtils->getWritablePath();  
        std::string textfieldtxt = std::string(textfield->getString());
        if(textfieldtxt.empty()){
            return;
        }
        std::string folder = systemPath + "projects/" + textfieldtxt + "/";
        if(fileUtils->isDirectoryExist(folder)){
            return;
        }
        std::string sounds = folder + "sounds/";
        std::string fonts = folder + "fonts/"; 
        std::string scenes = folder + "scenes/";
        std::string textures = folder + "textures/";
        fileUtils->createDirectories(folder);
        fileUtils->createDirectories(sounds);
        fileUtils->createDirectories(fonts);
        fileUtils->createDirectories(scenes);
        fileUtils->createDirectories(textures);
        
        if(fileUtils->isDirectoryExist(folder)){
            DATAmanager::getInstance()->projectPath = folder;
            DATAmanager::getInstance()->projectName = textfieldtxt;
            std::string IconPlaceHolder = "sprites/icon_scene_placeholder.png";
            std::string file = folder + "icon.png";
            Data fileData = fileUtils->getDataFromFile(IconPlaceHolder);
            fileUtils->writeDataToFile(fileData,file);
            std::string saveSequance = folder + "scenes/scenes.plist";
            ValueVector sequance;
            fileUtils->writeValueVectorToFile(sequance,saveSequance);

            saveSequance = folder + "sounds/audio.plist";
            fileUtils->writeValueVectorToFile(sequance,saveSequance);

            saveSequance = folder + "fonts/fonts.plist";
            fileUtils->writeValueVectorToFile(sequance,saveSequance);

            saveSequance = folder + "textures/textures.plist";
            fileUtils->writeValueVectorToFile(sequance,saveSequance);

            std::string savePj = systemPath + "projects/projects.plist";
            ValueVector pj = fileUtils->getValueVectorFromFile(savePj);
            pj.push_back(Value(textfieldtxt));
            fileUtils->writeValueVectorToFile(pj,savePj);

            auto nextScene = Scenes::createScene();
            Director::getInstance()->replaceScene(nextScene);
        }
});
    scheduleUpdate();

    return true;
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
        break;
    }

    }  // switch
}
