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
    std::string name;
    std::string command;
};

class DATAmanager 
{
    public:
    static DATAmanager* getInstance();

    std::vector<int> settingStorage; 
    std::vector<std::string> listCommands;
    std::vector<std::string> listExamples;
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
    std::string fontName = "";
    std::string videoName = "";
    bool LanguageLoad = false;
    bool ThemeLoad = false;
    bool VSync = false;
    bool stats = true;
    int fps = 60;
    //MainScene
    int theme = 0;
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
    ax::ui::Layout* panel = nullptr;
    ax::Node* panelNode = nullptr;
    ax::EventListenerTouchOneByOne* panelTouch = nullptr;
    int startPoint = 0;
    int endPoint = 0;
    //CodeHelper
    int bookName = 0;
    private:
    DATAmanager() = default;
};

