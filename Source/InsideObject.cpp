#include "ui/CocosGUI.h"
#include <string>
#include "axmol.h"
#include "src/DATAmanager.h"
#include "InsideObject.h"
#include "InsideScene.h"
#include "CodeHelper.h"
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace ax;

void SaveCode(std::vector<std::string> &code);

void findWord(Label* line, int selectedSymbol){
    auto letter = line->getLetter(selectedSymbol);
    if(letter == nullptr && selectedSymbol > 0){
        selectedSymbol--;
        letter = line->getLetter(selectedSymbol);
        if(letter == nullptr || selectedSymbol <= 0){
            return;
        }
    }
    while(letter != nullptr && selectedSymbol > 0){
        selectedSymbol--;
        letter = line->getLetter(selectedSymbol);
    }
    int start = selectedSymbol;
    selectedSymbol++;
    letter = line->getLetter(selectedSymbol);
    auto stringLength = line->getStringLength();
    while(letter != nullptr && selectedSymbol < stringLength){
        selectedSymbol++;
        letter = line->getLetter(selectedSymbol);
    }
    int end = selectedSymbol;
    if(selectedSymbol == stringLength && selectedSymbol > 0){
        end = stringLength;
    }

    auto text = line->getString();
    auto word = text.substr(start,end - start);
    std::cout<<word<<std::endl;
}

void DrawCursor(Label* line,int selectedChar){
    auto oldCursor = DATAmanager::getInstance()->cursor;
    bool rightMode = DATAmanager::getInstance()->rightMode;
    if(oldCursor != nullptr && oldCursor->getParent() != nullptr){
        oldCursor->removeFromParent();
        DATAmanager::getInstance()->cursor = nullptr;
    }
    auto cursorDraw = DrawNode::create();
    float cursorX = 0;
    float cursorY = 40;
    int totalSymbol = line->getStringLength();
    //std::cout<<"Total: "<<totalSymbol<<std::endl;
    //std::cout<<"Chosen: "<<selectedChar<<std::endl;
    if(totalSymbol > 0){
        if(selectedChar < totalSymbol){
            auto letter = line->getLetter(selectedChar);
            if(letter != nullptr){
                if(!rightMode){
                    cursorX = letter->getPositionX() - letter->getContentSize().width/2;
                } else {
                    cursorX = letter->getPositionX() + letter->getContentSize().width/2;
                }
            } else {
                if(selectedChar > 0){
                    auto dis = line->getLetter(selectedChar - 1);
                    cursorX = dis->getPositionX() + dis->getContentSize().width/2;
                }
            }
        } else {
            auto letter = line->getLetter(totalSymbol);
            if(letter != nullptr){
                if(!rightMode){
                    cursorX = letter->getPositionX() - letter->getContentSize().width/2;
                } else {
                    cursorX = letter->getPositionX() + letter->getContentSize().width/2;
                }
            } else {
                auto dis = line->getLetter(totalSymbol - 1);
                if(dis != nullptr){
                    cursorX = dis->getPositionX() + dis->getContentSize().width/2;
                }
            }
        }
    }
    cursorDraw->drawSegment(Vec2(cursorX, cursorY + 30), Vec2(cursorX, cursorY - 30), 6.5f, Color4F::BLUE);
    cursorDraw->setTag(70921);
    line->addChild(cursorDraw);

    cursorDraw->setOpacity(255);
    auto fadeOut = FadeOut::create(0.5f);
    auto fadeIn = FadeIn::create(0.5f);
    auto animationSeq = Sequence::create(fadeOut,fadeIn,nullptr);
    cursorDraw->runAction(RepeatForever::create(animationSeq));

    DATAmanager::getInstance()->cursor = cursorDraw;
    findWord(line,selectedChar);
}

void typeSymbol(std::string input,std::vector<std::string>& code,ui::ScrollView* codeList){
    bool rightMode = DATAmanager::getInstance()->rightMode;
    int selectedSymbol = DATAmanager::getInstance()->selectedChar;
    int selectedLine = DATAmanager::getInstance()->selectedLine;
    std::cout<<codeList->getInnerContainerPosition().x<<std::endl;

    if(input == "Space"){
        code[selectedLine].insert(selectedSymbol," ");
        DATAmanager::getInstance()->selectedChar += 1;
        auto lineLabel = dynamic_cast<Label*>(codeList->getChildByTag(selectedLine));
        if(lineLabel != nullptr){
        lineLabel->setString(code[selectedLine]);
        auto cursor = dynamic_cast<DrawNode*>(lineLabel->getChildByTag(70921));
        if(cursor != nullptr){
            auto pos = cursor->getPositionX();
            cursor->setPositionX(pos + 25);
        }
        auto total = codeList->getInnerContainerSize().height;
        auto visibleSize = codeList->getContentSize().width;
        auto totalw = (lineLabel->getContentSize().width * 1.2f) + 150;
        if(totalw < visibleSize){
            totalw = visibleSize;
        }
        codeList->setInnerContainerSize(Size(totalw,total));
        return;
    }
}
    if(input == "Erase"){
        if(selectedSymbol > 0){
            selectedSymbol -= 1;
            code[selectedLine].erase(selectedSymbol,1);
            DATAmanager::getInstance()->selectedChar -= 1;
            auto lineLabel = dynamic_cast<Label*>(codeList->getChildByTag(selectedLine));
            auto letter = lineLabel->getLetter(selectedSymbol);
            auto lastLetter = lineLabel->getLetter(selectedSymbol - 1);
            lineLabel->setString(code[selectedLine]);
            auto cursor = dynamic_cast<DrawNode*>(lineLabel->getChildByTag(70921));
            if(cursor != nullptr){
                if(letter != nullptr){
                    auto pos = cursor->getPositionX();
                    auto width = letter->getContentSize().width * 1.1f;
                    cursor->setPositionX(pos - width);
                    if(lastLetter != nullptr){
                        DrawCursor(lineLabel,selectedSymbol);
                    }
                } else {
                    auto pos = cursor->getPositionX();
                    cursor->setPositionX(pos - 25);  
                }    
            }
            } else if(selectedLine > 0){
                int neighborLine = selectedLine - 1;
                int updateSymbol = code[neighborLine].length();
                code[neighborLine] += code[selectedLine];
                code.erase(code.begin() + selectedLine);
                DATAmanager::getInstance()->selectedLine = selectedLine - 1;
                DATAmanager::getInstance()->selectedChar = selectedSymbol + updateSymbol;
                SaveCode(code);
                auto reload = InsideObject::createScene();
                Director::getInstance()->replaceScene(reload);
            }
        return;
    }
    if(input == "Enter"){
        std::string lineText = code[selectedLine];
        std::string leftPart = lineText.substr(0,selectedSymbol);
        std::string rightPart = lineText.substr(selectedSymbol);
        code[selectedLine] = leftPart;
        code.insert(code.begin() + selectedLine + 1,rightPart);
        DATAmanager::getInstance()->selectedLine = selectedLine + 1;
        DATAmanager::getInstance()->selectedChar = 0;
        SaveCode(code);
        auto reload = InsideObject::createScene();
        Director::getInstance()->replaceScene(reload);
        return;
    }
    code[selectedLine].insert(selectedSymbol,input);
    DATAmanager::getInstance()->selectedChar += 1;
    auto lineData = dynamic_cast<Label*>(codeList->getChildByTag(selectedLine));
    if(lineData != nullptr){
        lineData->setString(code[selectedLine]);
        DrawCursor(lineData,DATAmanager::getInstance()->selectedChar);
    }
    auto lineLabel = dynamic_cast<Label*>(codeList->getChildByTag(selectedLine));
    auto total = codeList->getInnerContainerSize().height;
    auto visibleSize = codeList->getInnerContainerSize().width;
    auto symbol = lineLabel->getLetter(DATAmanager::getInstance()->selectedChar - 1);
    auto totalSymbols = lineLabel->getStringLength();
    if(symbol == nullptr){
        return;
    }
    auto updateX = codeList->getContentSize().width/2 - symbol->getPositionX() - symbol->getContentSize().width;
    auto saveY = codeList->getInnerContainerPosition().y;
    auto totalw = (lineLabel->getContentSize().width * 1.2f) + 150;
    if(totalw < visibleSize){
        totalw = visibleSize;
    }
    codeList->setInnerContainerSize(Size(totalw,total));
    if(codeList->getInnerContainerPosition().x > updateX - 100){
        codeList->setInnerContainerPosition(Vec2(updateX,saveY));
    }
}

void SaveCode(std::vector<std::string> &code){
    std::string codePath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + DATAmanager::getInstance()->objectName + "/code.cx";
    std::ofstream output(codePath.data());
    for(const auto& line : code){
        output << line << "\n";
    }
    output.close();
}
Scene* InsideObject::createScene() 
{
    return InsideObject::create();
}

bool InsideObject::init()
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
    std::string keys = "qwertyuiopasdfghjklzxcvbnm";
    std::string bigSymbols = "QWERTYUIOPASDFGHJKLZXCVBNM";
    std::string numbers = "1234567890";
    std::string specialKeys = "!?<>.,-=()";
    capsLock = true;

    Color3B bgColor(24,28,80);
    auto layer = LayerColor::create(Color4B(bgColor),visibleSize.width,visibleSize.height);
    layer->setPosition(Vec2::ZERO);
    this->addChild(layer,0);

    auto DrawNode = DrawNode::create();
    auto color = Color4F(0,0.29f,0.71f,1.0f);
    DrawNode->drawSolidRect(Vec2(0,visibleSize.height), Vec2(visibleSize.width,visibleSize.height - 100),color);
    this->addChild(DrawNode,20);

    auto back = ui::Button::create("sprites/button_back.png"sv,"sprites/button_back_pressed.png"sv);
    back->setScale(0.2f);
    auto backSize = back->getContentSize();
    back->setPosition(Vec2(origin.x + 50,visibleSize.height - 50));
    back->addClickEventListener([=](Object* sender){
        DATAmanager::getInstance()->cursor = nullptr;
        DATAmanager::getInstance()->selectedChar = 0;
        DATAmanager::getInstance()->selectedLine = 0;
        SaveCode(code);
        auto nextScene = InsideScene::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });
    this->addChild(back, 25);

    codeList = ui::ScrollView::create();
    codeList->setContentSize(Size(visibleSize.width,visibleSize.height - 100));
    codeList->setPosition(Vec2(origin.x,origin.y));
    codeList->setDirection(ui::ScrollView::Direction::BOTH);
    codeList->setScrollBarEnabled(true);
    this->addChild(codeList);

    auto fileUtils = FileUtils::getInstance();
    std::string codePath = DATAmanager::getInstance()->projectPath + "scenes/" + DATAmanager::getInstance()->sceneName + "/" + DATAmanager::getInstance()->objectName + "/code.cx";
    code.clear();
    if(fileUtils->isFileExist(codePath)){
        std::string fullCode = fileUtils->getStringFromFile(codePath);
        std::stringstream ss(fullCode);
        std::string sliceLine;

        while(std::getline(ss,sliceLine)){
            code.push_back(sliceLine);
        }
        if(code.empty()){
            code.push_back("");
        }
    }
    std::string fullCode = fileUtils->getStringFromFile(codePath);
    auto containerSizer = Label::createWithBMFont("fonts/classic.fnt",fullCode);

    float distance = 120;
    float total = distance * 1.2f * code.size();
    if(total < visibleSize.height - 100){
        total = visibleSize.height - 100;
    }

    auto totalw = (containerSizer->getContentSize().width * 1.2f) + 150;
    if(totalw < visibleSize.width){
        totalw = visibleSize.width;
    }
    codeList->setInnerContainerSize(Size(totalw,total));
    if(DATAmanager::getInstance()->selectedLine == 0){
        codeList->setInnerContainerPosition(Vec2(0,((visibleSize.height - 100) - total)));
    } else {
        auto liner = dynamic_cast<Label*>(codeList->getChildByTag(DATAmanager::getInstance()->selectedLine));
        if(liner != nullptr){
            codeList->setInnerContainerPosition(Vec2(0,((liner->getPositionY() + 100) - total)));
        }
    }

    for(int i = 0;i < code.size(); i++){
        auto keyTouchListener = EventListenerTouchOneByOne::create();
        //keyTouchListener->setSwallowTouches(true);
        float y = total - (90 * i) - 150;
        auto line = Label::createWithBMFont("fonts/classic.fnt",code[i]);
        line->setAnchorPoint(Vec2::ZERO);
        line->setPosition(Vec2(100,y));
        line->setScale(1.2f);
        line->setTextColor(Color4B(255,255,255,255));
        line->setTag(i);
        codeList->addChild(line);
        auto dash = DrawNode::create();
        dash->drawSegment(Vec2(80,y + 75),Vec2(80,y - 10),8,Color4F(0,0.63f,0.85f,1));
        codeList->addChild(dash);
        std::string count = std::to_string(i);
        auto lineCount = Label::createWithTTF(count,"fonts/TT Rounds Neue.ttf",65 - count.length() * 2);
        lineCount->setTextColor(Color4B(0,161,217,255));
        lineCount->setPosition(Vec2(40,y + 42));
        codeList->addChild(lineCount);
        keyTouchListener->onTouchBegan = [line,i,y,this](Touch* touch,Event* event) -> bool {
            auto lineW = line->getContentSize().width;
            if(lineW < 25){
                lineW = 25;
            }
            Vec2 clickPoint = line->convertToNodeSpace(touch->getLocation());
            Rect rect = Rect(0,0,lineW,75);
            if(rect.containsPoint(clickPoint)){
                DATAmanager::getInstance()->selectedLine = i;
                float selectX = clickPoint.x;
                int totalSymbols = line->getStringLength();
                DATAmanager::getInstance()->selectedChar = 0;
                if(totalSymbols > 0){
                    float distance = 1000000.0f;
                    for(int c = 0;c < totalSymbols;c++){
                        auto letter = line->getLetter(c);
                        if(letter != nullptr){
                            float letterLX = letter->getPositionX();
                            float letterW = letter->getContentSize().width/2;
                            float letterRX = letterLX + letterW;

                            if(std::abs(selectX - letterLX) < distance){
                                distance = std::abs(selectX - letterLX);
                                DATAmanager::getInstance()->selectedChar = c;
                            }
                            if(std::abs(selectX - letterRX) < distance){
                                distance = std::abs(selectX - letterRX);
                                DATAmanager::getInstance()->selectedChar = c + 1;
                            }
                            if(std::abs(selectX - letterLX) < std::abs(selectX - letterRX)){
                                DATAmanager::getInstance()->rightMode = false;
                            } else {
                                DATAmanager::getInstance()->rightMode = true;
                            }
                        } else {
                            float letterLX = 0;
                            float letterRX = 25;
                            if(std::abs(selectX - letterLX) < distance){
                                distance = std::abs(selectX - letterLX);
                                DATAmanager::getInstance()->selectedChar = c;
                            }
                            if(std::abs(selectX - letterRX) < distance){
                                distance = std::abs(selectX - letterRX);
                                DATAmanager::getInstance()->selectedChar = c + 1;
                            }
                            if(std::abs(selectX - letterLX) < std::abs(selectX - letterRX)){
                                DATAmanager::getInstance()->rightMode = false;
                            } else {
                                DATAmanager::getInstance()->rightMode = true;
                            }
                        }
                    }
                    DrawCursor(line,DATAmanager::getInstance()->selectedChar);
                } else if(i < code.size()){
                    DrawCursor(line,0);
                }
                return true;
            }
            return false;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(keyTouchListener,line);
    }
    auto startLine = dynamic_cast<Label*>(codeList->getChildByTag(DATAmanager::getInstance()->selectedLine));
    DrawCursor(startLine,DATAmanager::getInstance()->selectedChar);

    //KEYBOARD SETTINGS

    auto create = ui::Button::create("sprites/keyboard_button_create.png","sprites/keyboard_button_create_pressed.png");
    create->setPosition(Vec2(visibleSize.width - 80,origin.y + 800));
    create->setScale(0.2f);
    create->setTag(188);
    this->addChild(create);
    create->addClickEventListener([this](Object* sender){
        DATAmanager::getInstance()->cursor = nullptr;
        SaveCode(code);
        auto nextScene = CodeHelper::createScene();
        Director::getInstance()->replaceScene(nextScene);
    });

    auto touchPannel = EventListenerTouchOneByOne::create();
    touchPannel->setSwallowTouches(true);

    auto keyboardPannel = Sprite::create("sprites/keyboard_panel.png");
    keyboardPannel->setPosition(Vec2(visibleSize.width/2,origin.y + 625));
    keyboardPannel->setTag(190);
    this->addChild(keyboardPannel);
    touchPannel->onTouchBegan = [keyboardPannel](Touch* touch, Event* event)->bool {
        bool hide = DATAmanager::getInstance()->hideKeyboard;
        Vec2 clickPoint = keyboardPannel->convertToNodeSpace(touch->getLocation());
        Rect rect = Rect(0,-600,keyboardPannel->getContentSize().width,keyboardPannel->getContentSize().height);
        if(rect.containsPoint(clickPoint)){
            if(!hide){
                return true;
            } else {
                return false;
            }
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchPannel,keyboardPannel);

    auto keyboardButton = ui::Button::create("sprites/keyboard_panel_button.png","sprites/keyboard_panel_button_pressed.png");
    keyboardButton->setPosition(Vec2(visibleSize.width/2,origin.y + 700));
    keyboardButton->setTag(191);
    this->addChild(keyboardButton);

    for(int i = 0;i < 10;i++){
        float x = 71 * i;
        std::string numString(1, numbers[i]);
        auto num = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        num->setPosition(Vec2(42 + x,origin.y + 520));
        num->setScale(0.14f);
        num->setScaleX(0.16f);
        num->setTag(1000 + i);
        this->addChild(num);
        num->addClickEventListener([numString,this](Object* sender){
            typeSymbol(numString,code,codeList);
        });

        auto numLabel = Label::createWithTTF(numString,"fonts/arial.ttf",35);
        numLabel->setPosition(Vec2(42 + x,origin.y + 520));
        numLabel->setTag(1010 + i);
        this->addChild(numLabel);

        std::string numString1(1,specialKeys[i]);
        auto num1 = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        num1->setPosition(Vec2(42 + x,origin.y + 520));
        num1->setVisible(false);
        num1->setScale(0.14f);
        num1->setScaleX(0.16f);
        num1->setTag(100 + i);
        this->addChild(num1);
        num1->addClickEventListener([numString1,this](Object* sender){
            typeSymbol(numString1,code,codeList);
        });

        auto numLabel1 = Label::createWithTTF(numString1,"fonts/arial.ttf",35);
        numLabel1->setVisible(false);
        numLabel1->setPosition(Vec2(42 + x,origin.y + 520));
        numLabel1->setTag(110 + i);
        this->addChild(numLabel1);
    }
    for(int i = 0;i < 10;i++){
        float x = 71 * i;
        std::string keyString(1,keys[i]);
        auto key = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        key->setPosition(Vec2(42 + x,origin.y + 420));
        key->setScale(0.14f);
        key->setScaleX(0.16f);
        key->setTag(1020 + i);
        this->addChild(key);
        key->addClickEventListener([keyString,this](Object* sender){
            typeSymbol(keyString,code,codeList);
        });

        auto keyNum = Label::createWithTTF(keyString,"fonts/arial.ttf",35);
        keyNum->setPosition(Vec2(42 + x,origin.y + 420));
        keyNum->setTag(1030 + i);
        this->addChild(keyNum);

        std::string numString1(1,bigSymbols[i]);
        auto num1 = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        num1->setVisible(false);
        num1->setPosition(Vec2(42 + x,origin.y + 420));
        num1->setScale(0.14f);
        num1->setScaleX(0.16f);
        num1->setTag(120 + i);
        this->addChild(num1);
        num1->addClickEventListener([numString1,this](Object* sender){
            typeSymbol(numString1,code,codeList);
        });

        auto numLabel1 = Label::createWithTTF(numString1,"fonts/arial.ttf",35);
        numLabel1->setVisible(false);
        numLabel1->setPosition(Vec2(42 + x,origin.y + 420));
        numLabel1->setTag(130 + i);
        this->addChild(numLabel1);
    }
    for(int i = 0;i < 9;i++){
        float x = 72 * i;
        int keyValue = 10 + i;
        std::string keyString(1,keys[keyValue]);
        auto key = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        key->setPosition(Vec2(68 + x,origin.y + 318));
        key->setScale(0.14f);
        key->setScaleX(0.16f);
        key->setTag(1040 + i);
        this->addChild(key);
        key->addClickEventListener([keyString,this](Object* sender){
            typeSymbol(keyString,code,codeList);
        });

        auto keyNum = Label::createWithTTF(keyString,"fonts/arial.ttf",35);
        keyNum->setPosition(Vec2(68 + x,origin.y + 318));
        keyNum->setTag(1050 + i);
        this->addChild(keyNum);

        std::string numString1(1,bigSymbols[keyValue]);
        auto num1 = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        num1->setVisible(false);
        num1->setPosition(Vec2(68 + x,origin.y + 318));
        num1->setScale(0.14f);
        num1->setScaleX(0.16f);
        num1->setTag(140 + i);
        this->addChild(num1);
        num1->addClickEventListener([numString1,this](Object* sender){
            typeSymbol(numString1,code,codeList);
        });

        auto numLabel1 = Label::createWithTTF(numString1,"fonts/arial.ttf",35);
        numLabel1->setVisible(false);
        numLabel1->setPosition(Vec2(68 + x,origin.y + 318));
        numLabel1->setTag(150 + i);
        this->addChild(numLabel1);
    }
    for(int i = 0;i < 7;i++){
        float x = 72 * i;
        int keyValue = 19 + i;
        std::string keyString(1,keys[keyValue]);
        auto key = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        key->setPosition(Vec2(140 + x,origin.y + 215));
        key->setScale(0.14f);
        key->setScaleX(0.16f);
        key->setTag(1060 + i);
        this->addChild(key);
        key->addClickEventListener([keyString,this](Object* sender){
            typeSymbol(keyString,code,codeList);
        });

        auto keyNum = Label::createWithTTF(keyString,"fonts/arial.ttf",35);
        keyNum->setPosition(Vec2(140 + x,origin.y + 215));
        keyNum->setTag(1070 + i);
        this->addChild(keyNum);

        std::string numString1(1,bigSymbols[keyValue]);
        auto num1 = ui::Button::create("sprites/keyboard_button.png","sprites/keyboard_button_pressed.png");
        num1->setVisible(false);
        num1->setPosition(Vec2(140 + x,origin.y + 215));
        num1->setScale(0.14f);
        num1->setScaleX(0.16f);
        num1->setTag(160 + i);
        this->addChild(num1);
        num1->addClickEventListener([numString1,this](Object* sender){
            typeSymbol(numString1,code,codeList);
        });

        auto numLabel1 = Label::createWithTTF(numString1,"fonts/arial.ttf",35);
        numLabel1->setVisible(false);
        numLabel1->setPosition(Vec2(140 + x,origin.y + 215));
        numLabel1->setTag(170 + i);
        this->addChild(numLabel1);
    }
    
    auto space = ui::Button::create("sprites/keyboard_button_space.png","sprites/keyboard_button_space_pressed.png");
    space->setPosition(Vec2(visibleSize.width/2,origin.y + 130));
    space->setScale(0.4f);
    space->setTag(180);
    this->addChild(space);
    space->addClickEventListener([this](Object* sender){
        std::string sym = "Space";
        typeSymbol(sym,code,codeList);
    });

    auto spaceLabel = Label::createWithTTF("Space","fonts/arial.ttf",35);
    spaceLabel->setPosition(Vec2(visibleSize.width/2,origin.y + 130));
    spaceLabel->setTag(181);
    this->addChild(spaceLabel);

    auto erase = ui::Button::create("sprites/keyboard_button_erase.png","sprites/keyboard_button_erase_pressed.png");
    erase->setPosition(Vec2(visibleSize.width - 56,origin.y + 218));
    erase->setScale(0.18f);
    erase->setScaleX(0.17f);
    erase->setTag(182);
    this->addChild(erase);
    erase->addClickEventListener([this](Object* sender){
        std::string sym = "Erase";
        typeSymbol(sym,code,codeList);
    });

    auto enter = ui::Button::create("sprites/keyboard_button_caps.png","sprites/keyboard_button_caps_pressed.png");
    enter->setPosition(Vec2(visibleSize.width - 56,origin.y + 130));
    enter->setScale(0.17f);
    enter->setScaleX(0.15f);
    enter->setTag(183);
    this->addChild(enter);
    enter->addClickEventListener([this](Object* sender){
        std::string sym = "Enter";
        typeSymbol(sym,code,codeList);
    });

    auto enterLab = Label::createWithTTF("Enter","fonts/arial.ttf",35);
    enterLab->setPosition(Vec2(visibleSize.width - 56,origin.y + 130));
    enterLab->setTag(184);
    this->addChild(enterLab);

    auto caps = ui::Button::create("sprites/keyboard_button_caps.png","sprites/keyboard_button_caps_pressed.png");
    caps->setPosition(Vec2(origin.x + 52,origin.y + 218));
    caps->setScale(0.18f);
    caps->setScaleX(0.16f);
    caps->setTag(185);
    this->addChild(caps);

    auto capsLab = Label::createWithTTF("Caps","fonts/arial.ttf",35);
    capsLab->setPosition(Vec2(origin.x + 52,origin.y + 218));
    capsLab->setTag(186);
    this->addChild(capsLab);
    caps->addClickEventListener([keys,numbers,this](Object* sender){
        if(capsLock){
            capsLock = false;
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
        } else { /////////////////////////////////////////////////////////////////////////
            capsLock = true;
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
        }
    });

    keyboardButton->addClickEventListener([origin,this](Object* sender){
        bool hide = DATAmanager::getInstance()->hideKeyboard;
        if(!hide){
            DATAmanager::getInstance()->hideKeyboard = true;
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            auto hSpace = dynamic_cast<ui::Button*>(this->getChildByTag(180));
            auto hSpaceL = dynamic_cast<Label*>(this->getChildByTag(181));
            auto hErase = dynamic_cast<ui::Button*>(this->getChildByTag(182));
            auto hEnter = dynamic_cast<ui::Button*>(this->getChildByTag(183));
            auto hEnterL = dynamic_cast<Label*>(this->getChildByTag(184));
            auto hCaps = dynamic_cast<ui::Button*>(this->getChildByTag(185));
            auto hCapsLab = dynamic_cast<Label*>(this->getChildByTag(186));
            auto hCreate = dynamic_cast<ui::Button*>(this->getChildByTag(188));
            auto hPanel = dynamic_cast<Sprite*>(this->getChildByTag(190));
            auto hButton = dynamic_cast<ui::Button*>(this->getChildByTag(191));
            hSpace->setVisible(false);
            hSpaceL->setVisible(false);
            hCaps->setVisible(false);
            hCapsLab->setVisible(false);
            hEnter->setVisible(false);
            hEnterL->setVisible(false);
            hErase->setVisible(false);
            hPanel->setVisible(false);
            hCreate->setPositionY(origin.y + 125);
            hButton->setPositionY(origin.y + 25);
        } else { //RESTART KEYBOARD with capsLock
            DATAmanager::getInstance()->hideKeyboard = false;
            if(!capsLock){
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
        } else { /////////////////////////////////////////////////////////////////////////
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
        }
        auto hSpace = dynamic_cast<ui::Button*>(this->getChildByTag(180));
            auto hSpaceL = dynamic_cast<Label*>(this->getChildByTag(181));
            auto hErase = dynamic_cast<ui::Button*>(this->getChildByTag(182));
            auto hEnter = dynamic_cast<ui::Button*>(this->getChildByTag(183));
            auto hEnterL = dynamic_cast<Label*>(this->getChildByTag(184));
            auto hCaps = dynamic_cast<ui::Button*>(this->getChildByTag(185));
            auto hCapsLab = dynamic_cast<Label*>(this->getChildByTag(186));
            auto hCreate = dynamic_cast<ui::Button*>(this->getChildByTag(188));
            auto hPanel = dynamic_cast<Sprite*>(this->getChildByTag(190));
            auto hButton = dynamic_cast<ui::Button*>(this->getChildByTag(191));
            hSpace->setVisible(true);
            hSpaceL->setVisible(true);
            hCaps->setVisible(true);
            hCapsLab->setVisible(true);
            hEnter->setVisible(true);
            hEnterL->setVisible(true);
            hErase->setVisible(true);
            hPanel->setVisible(true);
            hCreate->setPositionY(origin.y + 800);
            hButton->setPositionY(origin.y + 700);
        }
    });

    //if scene will be restarted, run the import settings keyboard

    bool hide = DATAmanager::getInstance()->hideKeyboard;
        if(hide){
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(false);
            }
            auto hSpace = dynamic_cast<ui::Button*>(this->getChildByTag(180));
            auto hSpaceL = dynamic_cast<Label*>(this->getChildByTag(181));
            auto hErase = dynamic_cast<ui::Button*>(this->getChildByTag(182));
            auto hEnter = dynamic_cast<ui::Button*>(this->getChildByTag(183));
            auto hEnterL = dynamic_cast<Label*>(this->getChildByTag(184));
            auto hCaps = dynamic_cast<ui::Button*>(this->getChildByTag(185));
            auto hCapsLab = dynamic_cast<Label*>(this->getChildByTag(186));
            auto hCreate = dynamic_cast<ui::Button*>(this->getChildByTag(188));
            auto hPanel = dynamic_cast<Sprite*>(this->getChildByTag(190));
            auto hButton = dynamic_cast<ui::Button*>(this->getChildByTag(191));
            hSpace->setVisible(false);
            hSpaceL->setVisible(false);
            hCaps->setVisible(false);
            hCapsLab->setVisible(false);
            hEnter->setVisible(false);
            hEnterL->setVisible(false);
            hErase->setVisible(false);
            hPanel->setVisible(false);
            hCreate->setPositionY(origin.y + 125);
            hButton->setPositionY(origin.y + 25);
        } else { //RESTART KEYBOARD with capsLock
            if(!capsLock){
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(true);
                key->setVisible(false);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(true);
                label->setVisible(false);
            }
        } else { /////////////////////////////////////////////////////////////////////////
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(100 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1000 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(110 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1010 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(120 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1020 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(130 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1030 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(140 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1040 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(150 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1050 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto keyChanger = dynamic_cast<ui::Button*>(this->getChildByTag(160 + i));
                auto key = dynamic_cast<ui::Button*>(this->getChildByTag(1060 + i));

                if(keyChanger == nullptr || key == nullptr){
                    continue;
                }
                keyChanger->setVisible(false);
                key->setVisible(true);
            }
            for(int i = 0;i < 10;i++){
                auto labelChanger = dynamic_cast<Label*>(this->getChildByTag(170 + i));
                auto label = dynamic_cast<Label*>(this->getChildByTag(1070 + i));

                if(labelChanger == nullptr || label == nullptr){
                    continue;
                }
                labelChanger->setVisible(false);
                label->setVisible(true);
            }
        }
        auto hSpace = dynamic_cast<ui::Button*>(this->getChildByTag(180));
            auto hSpaceL = dynamic_cast<Label*>(this->getChildByTag(181));
            auto hErase = dynamic_cast<ui::Button*>(this->getChildByTag(182));
            auto hEnter = dynamic_cast<ui::Button*>(this->getChildByTag(183));
            auto hEnterL = dynamic_cast<Label*>(this->getChildByTag(184));
            auto hCaps = dynamic_cast<ui::Button*>(this->getChildByTag(185));
            auto hCapsLab = dynamic_cast<Label*>(this->getChildByTag(186));
            auto hCreate = dynamic_cast<ui::Button*>(this->getChildByTag(188));
            auto hPanel = dynamic_cast<Sprite*>(this->getChildByTag(190));
            auto hButton = dynamic_cast<ui::Button*>(this->getChildByTag(191));
            hSpace->setVisible(true);
            hSpaceL->setVisible(true);
            hCaps->setVisible(true);
            hCapsLab->setVisible(true);
            hEnter->setVisible(true);
            hEnterL->setVisible(true);
            hErase->setVisible(true);
            hPanel->setVisible(true);
            hCreate->setPositionY(origin.y + 800);
            hButton->setPositionY(origin.y + 700);
        }

    scheduleUpdate();

    return true;
}

void InsideObject::update(float delta)
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

void InsideObject::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}
