//language.h
#pragma once

#include "axmol.h"
#include <string>
#include <map>

class LANGUAGE 
{
    public:
    static void changeLanguage(const std::string& languageText);

    private:
    LANGUAGE() = default;
};

