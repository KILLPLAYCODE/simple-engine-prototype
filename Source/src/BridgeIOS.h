#ifndef IOS_BRIDGE_H
#define IOS_BRIDGE_H

#include <string>

class IOSBridge {
public:
    static void importMediaFile(std::string type);
    
    static void onFileSelected(const std::string& cachePath, std::string fileName);
};

#endif
