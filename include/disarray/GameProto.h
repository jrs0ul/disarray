#pragma once

#include <cstdint>

class AAssetManager;
class OggStream;
class SpriteBatcher;
class SystemConfig;
class ShaderLoader;
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

    float           deltaTime;
    float           dT;
    float           accumulator;

    int             screenWidth;
    int             screenHeight;
    bool            works;

public:
    GameProto();
    ~GameProto();

    virtual void init() = 0;
    virtual void render() = 0;
    virtual void logic() = 0;
    virtual void destroy();

    int fps();

};
