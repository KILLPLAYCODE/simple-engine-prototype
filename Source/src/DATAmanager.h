//DATAmanager.h
#pragma once

#include "axmol.h"
#include <string>
#include <map>
#include "ui/CocosGUI.h"

class DATAmanager 
{
    public:
    static DATAmanager* getInstance();

    std::map<int,std::string> translater;

    std::string language = "en";
    std::string buttonFont = "";
    std::string titleFont = "";
    //std::string markerFont = "";
    float fontSize = 0;
    std::string savePath = "";
    std::string projectsFolder = "";
    std::string projectPath = "";
    std::string projectName = "";
    std::string sceneName = "";
    std::string objectName = "";
    std::string soundName = "";
    bool LanguageLoad = false;
    //InsideScene
    uint8_t switcher = 0;
    int sound_id = 0;
    ax::ui::Button* activePlayButton = nullptr;
    ax::ui::Button* activeStopButton = nullptr;
    //InsideObject
    bool hideKeyboard = false;
    int selectedLine = 0;
    int selectedChar = 0;
    bool rightMode = true;
    ax::DrawNode* cursor = nullptr;
    private:
    DATAmanager() = default;
};

