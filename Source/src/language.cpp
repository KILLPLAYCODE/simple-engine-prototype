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
    enText[11] = "CONDITIONS";
    enText[12] = "COSTUMES";
    enText[13] = "CYCLES";
    enText[14] = "EVENTS";
    enText[15] = "PHYSICS";
    enText[16] = "HITBOXES";
    enText[17] = "AUDIO";
    enText[18] = "SCENES";
    enText[19] = "CLONES";
    enText[20] = "VARIABLES";
    enText[21] = "LISTS";
    enText[22] = "MORE";
    enText[23] = "PROGRAMMER";
    enText[24] = "CREDITS";
    enText[25] = "OWNER";
    enText[26] = "ARTIST";
    enText[27] = "HELPER TRANSLATE";
    enText[28] = "CODER";
    enText[29] = "CONCEPT-MAKER";
    enText[30] = "DESIGNER";

    DATAmanager::getInstance()->language = "en";
    DATAmanager::getInstance()->translater = std::move(enText);
    DATAmanager::getInstance()->buttonFont = "fonts/Gokart Bubble.otf";
    DATAmanager::getInstance()->fontSize = 128;
    DATAmanager::getInstance()->titleFont = "fonts/TT Rounds Neue.ttf";
    //DATAmanager::getInstance()->titleFont = "fonts/Magic Yellow.otf";
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
    ruText[11] = "Условия";
    ruText[12] = "Костюмы";
    ruText[13] = "Циклы";
    ruText[14] = "События";
    ruText[15] = "Физика";
    ruText[16] = "Хитбоксы";
    ruText[17] = "Аудио";
    ruText[18] = "Сцены";
    ruText[19] = "Клоны";
    ruText[20] = "Переменные";
    ruText[21] = "Списки";
    ruText[22] = "Прочее";
    ruText[23] = "ПРОГРАМНИК";
    ruText[24] = "АВТОРЫ";
    ruText[25] = "ВЛАДЕЛЕЦ";
    ruText[26] = "ХУДОЖНИК";
    ruText[27] = "ЛОКАЛИЗАТОР";
    ruText[28] = "КОДЕР";
    ruText[29] = "КОНЦЕПТ-МЕЙКЕР";
    ruText[30] = "ДИЗАЙНЕР";

    DATAmanager::getInstance()->language = "ru";
    DATAmanager::getInstance()->translater = std::move(ruText);
    DATAmanager::getInstance()->buttonFont = "fonts/Simple.ttf";
    DATAmanager::getInstance()->fontSize = 128;
    DATAmanager::getInstance()->titleFont = "fonts/TT Rounds Neue.ttf";
    //DATAmanager::getInstance()->titleFont = "fonts/Ice Age SHA.otf";
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
    ukrText[11] = "Умови";
    ukrText[12] = "Костюми";
    ukrText[13] = "Цикли";
    ukrText[14] = "Події";
    ukrText[15] = "Фізика";
    ukrText[16] = "Хітбокси";
    ukrText[17] = "Аудіо";
    ukrText[18] = "Сцени";
    ukrText[19] = "Клони";
    ukrText[20] = "Змінні";
    ukrText[21] = "Списки";
    ukrText[22] = "Інше";
    ukrText[23] = "ПІДРУЧНИК";
    ukrText[24] = "АВТОРИ";
    ukrText[25] = "Власник";
    ukrText[26] = "ХУДОЖНИК";
    ukrText[27] = "ЛОКАЛІЗАТОР";
    ukrText[28] = "КОДЕР";
    ukrText[29] = "КОНЦЕПТ-МЕЙКЕР";
    ukrText[30] = "ДИЗАЙНЕР";
    
    DATAmanager::getInstance()->language = "ukr";
    DATAmanager::getInstance()->translater = std::move(ukrText);
    DATAmanager::getInstance()->buttonFont = "fonts/Simple.ttf";
    DATAmanager::getInstance()->fontSize = 128;
    DATAmanager::getInstance()->titleFont = "fonts/TT Rounds Neue.ttf";
    //DATAmanager::getInstance()->titleFont = "fonts/Ice Age SHA.otf";
    //DATAmanager::getInstance()->markerFont = "fonts/SPORE-Regular.ttf";
}