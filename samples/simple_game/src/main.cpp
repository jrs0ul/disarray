#include <disarray/SDLVideo.h>
#include <disarray/OSTools.h>
#include <disarray/TouchData.h>
#include <ctime>
#include "Game.h"


SDLVideo SDL;
SDL_Joystick *Joy = 0;
int JoyX = 0;
int JoyY = 0;
int MouseX, MouseY; //relative mouse coords
int _MouseX, _MouseY;
unsigned long tick;

Game game;


static void  process_events()
{

    SDL_Event event;
    float scaleX = 640.0f / game.screenWidth;
    float scaleY = 360.0f / game.screenHeight;

    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {

        case SDL_KEYDOWN:{

            game.globalKEY = (char)event.key.keysym.scancode;
            switch( event.key.keysym.sym ) {
                default:{} break;
                case SDLK_q: { game.works = false;} break;
                case SDLK_F1: {++game.DebugMode; if (game.DebugMode >= 3) game.DebugMode = 0;} 
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
//--------------------
void CheckKeys()
{
    float scaleX = 640.0f / game.screenWidth;
    float scaleY = 360.0f / game.screenHeight;
    int JoyNum = 0;

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    JoyNum = SDL_NumJoysticks();

    if (JoyNum > 0) 
    {
        SDL_JoystickOpen(0);

        SDL_JoystickUpdate ();
        JoyX = SDL_JoystickGetAxis(Joy, 0);
        JoyY = SDL_JoystickGetAxis(Joy, 1);
    }

    SDL_GetRelativeMouseState ( &MouseX,&MouseY );
    SDL_GetMouseState(&_MouseX, &_MouseY);

    game.gamepad.v[0] = JoyX / 1000.0f;
    game.gamepad.v[1] = JoyY / 1000.0f;
    game.MouseX = _MouseX * scaleX;
    game.MouseY = _MouseY * scaleY;

    memcpy(game.OldKeys, game.Keys, 20);
    memset(game.Keys, 0, 20);

    if ( keys[SDL_SCANCODE_W] )     game.Keys[0] = 1;
    if ( keys[SDL_SCANCODE_S] )     game.Keys[1] = 1;
    if ( keys[SDL_SCANCODE_A] )     game.Keys[2] = 1;
    if ( keys[SDL_SCANCODE_D] )     game.Keys[3] = 1;
    if ( keys[SDL_SCANCODE_UP] )    game.Keys[0] = 1;
    if ( keys[SDL_SCANCODE_DOWN])   game.Keys[1] = 1;
    if ( keys[SDL_SCANCODE_LEFT])   game.Keys[2] = 1;
    if ( keys[SDL_SCANCODE_RIGHT])  game.Keys[3] = 1;
    if ( keys[SDL_SCANCODE_SPACE])  game.Keys[4] = 1;
    if ( keys[SDL_SCANCODE_RETURN]) game.Keys[5] = 1;
    if ( keys[SDL_SCANCODE_ESCAPE]) game.Keys[6] = 1;
    if ( keys[SDL_SCANCODE_DELETE]) game.Keys[7] = 1;

    if (JoyNum)
    {
        if (SDL_JoystickGetButton (Joy, 0))
            game.Keys[4] = 1;
        if (SDL_JoystickGetButton (Joy, 1))
            game.Keys[5] = 1;

        SDL_JoystickClose(0);
    }
}
//--------------------
int main( int   argc, char *argv[] )
{
    srand(time(0));

    char buf[255];
    GetHomePath(buf);
    sprintf(game.documentPath, "%s.atari2600game", buf);
    MakeDir(game.documentPath);
    game.loadConfig();

    printf("%d %d\n", game.screenWidth, game.screenHeight);
    SDL.setMetrics(game.screenWidth, game.screenHeight);

    const char* title = "ATARI 2600 game proto";

    if (!SDL.initWindow(title, "icon.bmp", game.windowed, false))
    {
        game.works = false;
    }

    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    if(SDL_NumJoysticks() > 0)
    {
        Joy = SDL_JoystickOpen(0);
    }

    game.init();
    game.timeTicks = SDL_GetTicks();


    while (game.works)
    {
        if ((SDL_GetTicks() > tick))
        {
            game.deltaTime = (SDL_GetTicks() - game.timeTicks) / 1000.0f;
            game.timeTicks = SDL_GetTicks();

            game.accumulator += game.deltaTime;

            while (game.accumulator >= game.dT)
            {
                game.logic();
                game.accumulator -= game.dT;
            }

            CheckKeys();
            game.render();
            SDL.swap(false);

            tick = SDL_GetTicks() + 1000/61;
        }

        SDL_Delay(0.6);
        process_events();
    }

    game.destroy();
    SDL.quit(false);

    return 0;
}
