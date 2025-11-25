#pragma once

class GameProto;
class SDLVideo;
struct _SDL_GameController;


class disarray
{
    GameProto*           game;
    _SDL_GameController* gamepad;

public:
    disarray(GameProto* ng, _SDL_GameController* controller);

    void whileLoopPC(SDLVideo* sdl);
    void whileLoopAndroid();

    void handleInputPC();
    void handleInputAndroid();

    virtual void otherWhileLoopActivities() = 0;

};
