#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "InsideObject.h"
#include "src/DATAmanager.h"
#include "CodeHelper.h"
#include <map>
#include "src/language.h"
using namespace ax;

static int _sceneID = 1004;

Scene* CodeHelper::createScene() 
{
    return CodeHelper::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "CodeHelper.cpp\n");
}

bool CodeHelper::init()
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

    Color3B bgColor(30,110,110);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto DrawNode = DrawNode::create();    
    Color4F color = Color4F(0,0.28f,0.26f,1.0f);
    DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 150),color);
    this->addChild(DrawNode,20);

    auto label = Label::createWithTTF(text[23],LabelFont,75);
    label->setPosition(Vec2(visibleSize.width/2,visibleSize.height - 80));
    this->addChild(label,23);

    auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    back->setScale(0.2f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + 50,visibleSize.height - 50));
    this->addChild(back, 25);
    back->addClickEventListener([this](Object* sender){
        auto nextScene = InsideObject::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    auto list = ui::ScrollView::create();
    list->setContentSize(Vec2(visibleSize.width,visibleSize.height - 150));
    list->setPosition(Vec2(origin.x,origin.y));
    list->setDirection(ui::ScrollView::Direction::VERTICAL);
    list->setScrollBarEnabled(false);
    list->setBounceEnabled(true);
    this->addChild(list);
    int distance = 250;
    float total = distance * 12;
    if(total < visibleSize.height - 150){
        total = visibleSize.height - 150;
    }
    list->setInnerContainerSize(Size(visibleSize.width,total));
    float x = origin.x - 195;
    float y = total - 150;
    for(int i = 0;i < 12; i++){
        int string = 11 + i;
        auto categoryButton = ui::Button::create("sprites/button_category.png","sprites/button_category.png");
        auto categoryLabel = Label::createWithTTF(text[string],font, 45);
        categoryButton->setScale(0.46f);
        x += 370;
        if(x < visibleSize.width - 25){
            categoryButton->setPosition(Vec2(x,y - 50));
            categoryLabel->setPosition(Vec2(x, y - 175));
        } else {
            x = origin.x + 175;
            y -= 400;
            categoryButton->setPosition(Vec2(x,y - 50));
            categoryLabel->setPosition(Vec2(x, y - 175));
        }
        list->addChild(categoryButton);
        list->addChild(categoryLabel);
    }

    scheduleUpdate();

    return true;
}

void CodeHelper::update(float delta)
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

void CodeHelper::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}
