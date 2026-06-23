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
    std::string projectsFolder = "";
    std::string projectPath = "";
    std::string projectName = "";
    int GameScenes = 0;
    int GameObjects = 0;
    int Variables = 0;
    int Functions = 0;
    int Sounds = 0;
    int Fonts = 0;
    int Scenes = 0;
    private:
    DATAmanager() = default;
};

