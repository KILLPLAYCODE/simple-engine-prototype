#include "DATAmanager.h"
#include <string>
#define JSON_HAS_CPP_20 0
#include "json.hpp"
#include <fstream>
#include <unordered_map>
#include <iostream>
#include "axmol.h"
using json = nlohmann::json;

DATAmanager* DATAmanager::getInstance()
{
    static DATAmanager instance;
    return &instance;
}
void DATAmanager::loadSystemCommands(std::vector<SystemCommands> &commands,std::unordered_map<std::string,std::vector<SystemCommands>> &systemCommands){
    auto fileUtils = ax::FileUtils::getInstance();
    std::string pathJson = "codeEditorCM/systemLib.json";
    std::string systemJson = fileUtils->fullPathForFilename(pathJson);

    std::ifstream file(systemJson);
    if(!file.is_open()){
        std::cout<<"File do not exist"<<std::endl;
        return;
    }
    json list;
    file >> list;
    commands.clear();

    for (auto& j : list["categories"]) {
        std::string categoryName = j["name"];
        std::cout<<"\nCategory: " << categoryName << std::endl << std::endl;
        for (auto& s : j["commands"]) {
            SystemCommands sys;
            sys.name = s["name"];
            sys.command = s["example"];
            systemCommands[categoryName].push_back(sys);
            commands.push_back(sys);
            std::cout<<"Command name: " << sys.name << std::endl;
            std::cout<<"Command: " << sys.command << std::endl <<std::endl;
        }
    }
}