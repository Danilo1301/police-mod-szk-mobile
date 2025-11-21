#pragma once

#include "IDebug.h"
#include "IWindow.h"
#include "IWidget.h"
#include "IAudio.h"

#define RADAR_ICON_BIG 50.0f
#define RADAR_ICON_MEDIUM 25.0f
#define RADAR_ICON_SMALL 12.0f

#define GetTranslatedText menuSZK->GetLocalizationText

class IMenuSZK {
public:
    int deltaTime = 0;

    IDebug* debug = 0;

    IEventListener<>* onGameFirstUpdate;
    IEventListener<>* onGamePlayerReady;

    IEventListener<>* onBeforeMenuUpdate;
    IEventListener<>* onAfterMenuUpdate;
    
    IEventListener<>* onGameUpdate;

    IEventListener<>* onDrawBeforeMenu;
    IEventListener<>* onDrawAfterMenu;

    IEventListener<>* onPostDrawRadar;

    IEventListener<int>* onPedAdded;
    IEventListener<int>* onPedRemoved;
    IEventListener<int>* onVehicleAdded;
    IEventListener<int>* onVehicleRemoved;

    virtual IWindow* CreateWindow(float x, float y, float width, std::string title) = 0;
    virtual IWidget* CreateWidget(float x, float y, float size, std::string bgImage, std::string image) = 0;
    
    virtual IContainer* CreateContainer(std::string tag) = 0;
    virtual IContainer* GetRootContainer() = 0;

    virtual void SetGlobalIntVariable(const std::string& key, int value) = 0;
    virtual int GetGlobalIntVariable(const std::string& key) = 0;

    virtual void* GetCPedFromRef(int pedRef) = 0;
    virtual void* GetCVehicleFromRef(int vehicleRef) = 0;

    virtual void* LoadTexture(std::string src) = 0;
    virtual void DrawTextureOnRadar(void* texture, CVector worldPosition, CRGBA color, float size) = 0;
    virtual void DrawTextureOnWorld(void* texture, CVector worldPosition, CRGBA color, CVector2D size) = 0;

    virtual CVector2D ConvertWorldPositionToScreenPosition(CVector worldPosition) = 0;

    virtual std::string GetLocalizationsPath() = 0;
    virtual void RegisterLocalizationFolder(std::string folderPath) = 0;
    virtual std::string GetLocalizationText(std::string key) = 0;

    virtual IAudio* LoadAudio(std::string src) = 0;
    virtual IAudio* Load3DAudio(std::string src) = 0;
    virtual void UnloadAudio(IAudio* audio) = 0;
};