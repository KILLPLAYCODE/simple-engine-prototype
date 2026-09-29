#include "AppDelegate.h"
#include "MainScene.h"
#include <iostream>
#include "axmol.h"

#define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#    include "audio/AudioEngine.h"
#endif

using namespace ax;

static ax::Size designResolutionSize = ax::Size(720,1280);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

// if you want a different context, modify the value of gfxContextAttrs
// it will affect all platforms
void AppDelegate::initGfxContextAttrs()
{
    // set graphics context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GfxContextAttrs gfxContextAttrs = {8, 8, 8, 0, 24, 8, 0};
    // since axmol-2.2 vsync was enabled in engine by default

    auto fileUtils = FileUtils::getInstance();
    auto settingsPath = fileUtils->getWritablePath() + "settings.plist";
    if(fileUtils->isFileExist(settingsPath)){
        ValueVector settingsValue = fileUtils->getValueVectorFromFile(settingsPath);
        std::vector<int> settingss;
    
        for(const auto& value : settingsValue){
            settingss.push_back(value.asInt());
        }
    
        if(settingss[0] == 0){
            gfxContextAttrs.vsync = false;
            std::cout<<"FALSE\n";
        }
        if(settingss[0] == 1){
            gfxContextAttrs.vsync = true;
            std::cout<<"TRUE\n";
        }
    }
    
    RenderView::setGfxContextAttrs(gfxContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
    
    auto renderView = director->getRenderView();
    if (!renderView)
    {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
        renderView = RenderViewImpl::createWithRect(
            "Simple Engine", ax::Rect(0, 0, 360, 700));
#else
        renderView = RenderViewImpl::create("Simple Engine");
#endif
        director->setRenderView(renderView);
    }

    director->setStatsDisplay(false);

    auto fileUtils = FileUtils::getInstance();
    auto settingsPath = fileUtils->getWritablePath() + "settings.plist";
    if(fileUtils->isFileExist(settingsPath)){
        ValueVector settingsValue = fileUtils->getValueVectorFromFile(settingsPath);
        std::vector<int> settingss;
    
        for(const auto& value : settingsValue){
            settingss.push_back(value.asInt());
        }
    
        if(settingss[2] == 0){
            director->setStatsDisplay(false);
            std::cout<<"FALSE\n";
        }
        if(settingss[2] == 1){
            director->setStatsDisplay(true);
            std::cout<<"TRUE\n";
        }
    }

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60.0f);

    // Set the design resolution
    renderView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                    ResolutionPolicy::FIXED_WIDTH);

    // create a scene. it's an autorelease object
    auto scene = utils::createInstance<MainScene>();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}

void AppDelegate::applicationWillQuit() {}
