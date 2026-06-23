#include "DATAmanager.h"

DATAmanager* DATAmanager::getInstance()
{
    static DATAmanager instance;
    return &instance;
}