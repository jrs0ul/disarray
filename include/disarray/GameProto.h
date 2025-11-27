#pragma once

#include <cstdint>
#include <disarray/Vectors.h>

class AAssetManager;
class OggStream;
class SpriteBatcher;
class SystemConfig;
class ShaderLoader;
class VulkanVideo;
struct TouchData;


class GameProto
{

public:
    static const uint32_t MAX_DOCUMENT_PATH = 512;
    static const uint32_t GAME_KEY_COUNT    = 20;
    static const int      GAMEPAD_DEADZONE  = 10;

#ifdef __ANDROID__
    AAssetManager*  androidAssetManager;
#endif

    float           orthoMatrix[16];
    char            documentPath[MAX_DOCUMENT_PATH];
    uint8_t         oldKeys[GAME_KEY_COUNT];  //  old game keys
    uint8_t         keys[GAME_KEY_COUNT];     //  game keys
    Vector3D        gamepadLAxis;
    Vector3D        gamepadRAxis;
    long            timeTicks;
    long            tick;

    OggStream*      music;
    SpriteBatcher*  pics;
    SystemConfig*   sys;
    ShaderLoader*   shaders;
    TouchData*      touches;
    VulkanVideo*    vk;

    float           deltaTime;
    float           dT;
    float           accumulator;

    float           relativeMouseX;
    float           relativeMouseY;
    float           mouseX;
    float           mouseY;

    unsigned        screenWidth;
    unsigned        screenHeight;
    bool            windowed;
    bool            works;
    bool            doRumble;
    bool            hasVulkan;

public:
    GameProto();
    ~GameProto();

    virtual void init(bool useVulkan) = 0;
    virtual void loadConfig(const char* path);
    virtual void render() = 0;
    virtual void logic() = 0;
    virtual void destroy();
#ifdef __ANDROID__
    virtual void onBack() = 0;
#endif

    int fps();

};
