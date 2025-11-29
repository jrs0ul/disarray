#pragma once

#include <cstdint>

class GameProto;
class SDLVideo;
struct _SDL_GameController;


class disarray
{
    GameProto*           game;
    _SDL_GameController* gamepad;

public:
    disarray(GameProto* ng);

    void setupPC(SDLVideo* SDL,
                 uint32_t initialWidth,
                 uint32_t initialHeight,
                 const char* configPath,
                 const char* windowTitle,
                 bool initialUseVulkan);

    void runGamePC(SDLVideo* sdl);
    void runGameAndroid();

    void handleInputPC();
    void handleInputAndroid();

    void processEventsPc();
    void processEventsAndroid();

    virtual void otherWhileLoopActivities() = 0;

};
