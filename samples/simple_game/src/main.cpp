#include <disarray/SDLVideo.h>
#include <disarray/OSTools.h>
#include <disarray/TouchData.h>
#include <disarray/disarray.h>
#include <ctime>
#include "Game.h"

SDLVideo SDL;
Game game;

static void  process_events()
{
    SDL_Event event;
    float scaleX = 640.0f / game.screenWidth;
    float scaleY = 360.0f / game.screenHeight;

    while( SDL_PollEvent( &event ) )
    {
        switch( event.type ) {

        case SDL_KEYDOWN:{

            game.globalKEY = (char)event.key.keysym.scancode;
            switch( event.key.keysym.sym ) 
            {
                default:{} break;
                case SDLK_q: { game.works = false;} break;
            }
        } break;
        case SDL_MOUSEBUTTONUP:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            game.touches->up.push_back(pos);
            game.touches->allfingersup = true;
        } break;
        case SDL_MOUSEBUTTONDOWN:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            game.touches->down.push_back(pos);
            game.touches->allfingersup = false;

        } break;
        case SDL_MOUSEMOTION:{
            if(SDL_GetMouseState(0, 0)&SDL_BUTTON_LMASK){
                Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
                game.touches->move.push_back(pos);
                game.touches->allfingersup = false;
            }
        }break;

        case SDL_QUIT:{
            game.works = false;
        }break;

        }
    }
}

class DS : public disarray
{
public:
    DS(GameProto* ng, _SDL_GameController* controller) : disarray(ng, controller)
    {
    }
    void otherWhileLoopActivities() override {process_events();}
};

//--------------------
int main( int   argc, char *argv[] )
{
    srand(time(0));
    char buf[256];
    char finalBuf[1024];
    GetHomePath(buf);
    sprintf(game.documentPath, "%s.atari2600game", buf);
    MakeDir(game.documentPath);
    sprintf(finalBuf, "%s/settings.cfg", game.documentPath);
    game.loadConfig(finalBuf);

    printf("%d %d\n", game.screenWidth, game.screenHeight);
    SDL.setMetrics(game.screenWidth, game.screenHeight);

    const char* title = "ATARI 2600 game proto";

    if (!SDL.initWindow(title, "icon.bmp", game.windowed, false))
    {
        game.works = false;
    }

    SDL_GameController* gamepad = nullptr;
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    if(SDL_NumJoysticks() > 0)
    {
        if (SDL_IsGameController(0))
        {
            gamepad = SDL_GameControllerOpen(0);
        }
    }

    game.init(false);
    game.timeTicks = SDL_GetTicks();

    DS ds(&game, gamepad);

    ds.whileLoopPC(&SDL);

    game.destroy();
    SDL.quit(false);

    return 0;
}
