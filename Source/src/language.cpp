//language.cpp
#include "DATAmanager.h"
#include "language.h"
#include <string>
#include <map>
void loadEnglish();
void loadRussian();
void loadUkraine();

void LANGUAGE::changeLanguage(const std::string& languageText){
    if(languageText == "en"){
        loadEnglish();
    }
    if(languageText == "ru"){
        loadRussian();
    }
    if(languageText == "ukr"){
        loadUkraine();
    }
}
void loadEnglish(){
    std::map<int,std::string> enText;
    enText[0] = "CREATE";
    enText[1] = "MY PROJECTS";
    enText[2] = "PROJECT NAME";
    enText[3] = "Project name...";
    enText[4] = "Language:";
    enText[5] = "Scene name:";
    enText[6] = "Scene name...";
    enText[7] = "CANCEL";
    enText[8] = "DELETE";
    enText[9] = "OBJECT NAME";
    enText[10] = "Object name...";

    DATAmanager::getInstance()->language = "en";
    DATAmanager::getInstance()->translater = std::move(enText);
    DATAmanager::getInstance()->buttonFont = "fonts/Gokart Bubble.otf";
    DATAmanager::getInstance()->fontSize = 128;
    DATAmanager::getInstance()->titleFont = "fonts/Magic Yellow.otf";
    //DATAmanager::getInstance()->markerFont = "fonts/Marker Felt.ttf";
}
void loadRussian(){
    std::map<int,std::string> ruText;
    ruText[0] = "СОЗДАТЬ";
    ruText[1] = "МОИ ПРОЕКТЫ";
    ruText[2] = "НАЗВАНИЕ ПРОЕКТА";
    ruText[3] = "Название проекта...";
    ruText[4] = "Язык:";
    ruText[5] = "Название сцены:";
    ruText[6] = "Название сцены...";
    ruText[7] = "Отмена";
    ruText[8] = "Удалить";
    ruText[9] = "НАЗВАНИЕ ОБЪЕКТА";
    ruText[10] = "Название объекта...";

    DATAmanager::getInstance()->language = "ru";
    DATAmanager::getInstance()->translater = std::move(ruText);
    DATAmanager::getInstance()->buttonFont = "fonts/Simple.ttf";
    DATAmanager::getInstance()->fontSize = 128;
    DATAmanager::getInstance()->titleFont = "fonts/Ice Age SHA.otf";
    //DATAmanager::getInstance()->markerFont = "fonts/SPORE-Regular.ttf";
}
void loadUkraine(){
    std::map<int,std::string> ukrText;
    ukrText[0] = "СТВОРИТИ";
    ukrText[1] = "МОЇ ПРОЄКТИ";
    ukrText[2] = "НАЗВА ПРОЄКТУ";
    ukrText[3] = "Назва проєкту...";
    ukrText[4] = "Мова:";
    ukrText[5] = "Назва сцени:";
    ukrText[6] = "Назва сцени...";
    ukrText[7] = "Відміна";
    ukrText[8] = "Видалити";
    ukrText[9] = "НАЗВА ОБ'ЄКТУ";
    ukrText[10] = "Назва об'єкту";
    
    DATAmanager::getInstance()->language = "ukr";
    DATAmanager::getInstance()->translater = std::move(ukrText);
    DATAmanager::getInstance()->buttonFont = "fonts/Simple.ttf";
    DATAmanager::getInstance()->fontSize = 128;
    DATAmanager::getInstance()->titleFont = "fonts/Ice Age SHA.otf";
    //DATAmanager::getInstance()->markerFont = "fonts/SPORE-Regular.ttf";
}