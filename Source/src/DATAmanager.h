//DATAmanager.h
#pragma once

#include "axmol.h"
#include <string>
#include <map>
#include <unordered_map>
#include "ui/CocosGUI.h"

struct SystemCommands {
    std::string name;
    std::string command;
};
struct CustomLibs {
    std::string name;
    std::string description;
    std::string picture;
    std::string version;
};
struct CustomCommands {
    std::string command;
};

class DATAmanager 
{
    public:
    static DATAmanager* getInstance();

    std::vector<SystemCommands> commands;
    std::map<int,std::string> translater;
    std::unordered_map<std::string,std::vector<SystemCommands>> systemCommands;
    std::unordered_map<std::string,std::vector<CustomLibs>> customLibrary;
    std::unordered_map<std::string,std::vector<CustomCommands>> customCommands;

    static void loadSystemCommands(std::vector<SystemCommands> &commands,std::unordered_map<std::string,std::vector<SystemCommands>> &systemCommands);
    //static void autoPasteRun(std::vector<SystemCommands> &commands, std::unordered_map<std::string,std::vector<SystemCommands>> &systemCommands);

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
    //IOS
    int iosRequestSender = 0; //1 - ListProject, 2 - InsideScene
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
    ax::Vec2 scrollPosition = ax::Vec2::ZERO;
    //CodeHelper
    int bookName = 0;
    private:
    DATAmanager() = default;
};

