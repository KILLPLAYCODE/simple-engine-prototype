//DATAmeneger.h
#pragma once

#include "axmol.h"
#include <string>
#include <map>

class DATAmanager 
{
    public:
    static DATAmanager* getInstance();

    std::map<int,std::string> translater;

    std::string language = "ru";
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
    bool LanguageLoad = false;
    //InsideScene
    uint8_t switcher = 0;
    private:
    DATAmanager() = default;
};

