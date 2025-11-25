#pragma once

#include <cstdint>

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

#ifdef __ANDROID__
    AAssetManager*  androidAssetManager;
#endif

    float           orthoMatrix[16];
    char            documentPath[MAX_DOCUMENT_PATH];
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

    unsigned        screenWidth;
    unsigned        screenHeight;
    bool            works;
    bool            hasVulkan;

public:
    GameProto();
    ~GameProto();

    virtual void init(bool useVulkan) = 0;
    virtual void render() = 0;
    virtual void logic() = 0;
    virtual void destroy();

    int fps();

};
