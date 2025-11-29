#include <disarray/disarray.h>
#include <disarray/GameProto.h>
#include <disarray/SysConfig.h>
#include <disarray/TouchData.h>
#ifndef __ANDROID__
#include <disarray/SDLVideo.h>
#endif

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <SDL_opengl.h>
    #else
        #include <SDL2/SDL_opengl.h>
    #endif
#else
    #ifdef __ANDROID__
            #include <GLES2/gl2.h>
            #include <GLES2/gl2ext.h>
    #else
            #include <SDL2/SDL_opengl.h>
    #endif
#endif


const char* GamePadTypes[] = {"Unknown", "XBOX 360", "XBOX One", "Playstation 3",
                              "Playstation 4", "Nintendo Switch PRO", "Virtual",
                              "Playstation 5", "Amazon Luna", "Google Stadia",
                              "NVIDIA Shield", "Nintendo Switch Joycon LEFT",
                              "Nintendo Switch Joycon RIGHT", "Nintendo Switch Joycon PAIR"};


disarray::disarray(GameProto* ng)
{
    game = nullptr;

    if (ng)
    {
        game = ng;
    }
}


#ifndef __ANDROID__
void disarray::setupPC(SDLVideo* sdl,
                       uint32_t initialWidth,
                       uint32_t initialHeight,
                       bool     initialFullscreen,
                       const char* configPath,
                       const char* title,
                       bool initialUseVulkan)
{
    game->loadConfig(configPath, initialWidth, initialHeight, initialFullscreen, initialUseVulkan);
    sdl->setMetrics(game->screenWidth, game->screenHeight);

    if (!sdl->initWindow(title, "icon.bmp", game->windowed, (bool)game->sys->renderIdx))
    {
        game->works = false;
    }

    game->vk = sdl->getVkVideo();

    gamepad = nullptr;
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    if(SDL_NumJoysticks() > 0)
    {
        if (SDL_IsGameController(0))
        {
            SDL_GameControllerType controllerType = SDL_CONTROLLER_TYPE_UNKNOWN;
            controllerType = SDL_GameControllerTypeForIndex(0);
            gamepad = SDL_GameControllerOpen(0);
            printf("game controller type: %s\n", GamePadTypes[controllerType]);
        }
    }

    game->init(game->sys->renderIdx);

    if (game->sys->renderIdx == 0)
    {
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    game->timeTicks = SDL_GetTicks();
}


void disarray::runGamePC(SDLVideo* sdl)
{
    while (game->works)
    {
        if ((SDL_GetTicks() > game->tick))
        {
            game->deltaTime = (SDL_GetTicks() - game->timeTicks) / 1000.0f;
            game->timeTicks = SDL_GetTicks();

            game->accumulator += game->deltaTime;

            while (game->accumulator >= game->dT)
            {
                game->logic();
                game->accumulator -= game->dT;
            }

            handleInputPC();
            game->render();
            sdl->swap(game->hasVulkan);

            game->tick = SDL_GetTicks() + 1000 / 61;
        }

        processEventsPc();
        SDL_Delay(0.6);
        otherWhileLoopActivities();
    } // while

    printf("QUITING!\n");

    game->destroy();

    sdl->quit(game->sys->renderIdx);

}

void disarray::handleInputPC()
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    int32_t MouseX, MouseY, _MouseX, _MouseY;
    SDL_GetRelativeMouseState ( &MouseX, &MouseY );
    SDL_GetMouseState(&_MouseX, &_MouseY);

    game->relativeMouseX = (float)MouseX;
    game->relativeMouseY = (float)MouseY;

    game->mouseX = _MouseX;
    game->mouseY = _MouseY;


    memcpy(game->oldKeys, game->keys, GameProto::GAME_KEY_COUNT);
    memset(game->keys, 0, GameProto::GAME_KEY_COUNT);

    if ( keys[SDL_SCANCODE_W] )     game->keys[0] = 1;
    if ( keys[SDL_SCANCODE_S] )     game->keys[1] = 1;
    if ( keys[SDL_SCANCODE_A] )     game->keys[3] = 1;
    if ( keys[SDL_SCANCODE_D] )     game->keys[2] = 1;

    if ( keys[SDL_SCANCODE_UP] )    game->keys[0] = 1;
    if ( keys[SDL_SCANCODE_DOWN])   game->keys[1] = 1;
    if ( keys[SDL_SCANCODE_LEFT])   game->keys[3] = 1;
    if ( keys[SDL_SCANCODE_RIGHT])  game->keys[2] = 1;

    if ( keys[SDL_SCANCODE_SPACE])  game->keys[4] = 1;
    if ( keys[SDL_SCANCODE_RETURN]) game->keys[4] = 1;
    if ( keys[SDL_SCANCODE_ESCAPE]) game->keys[5] = 1;
    if ( keys[SDL_SCANCODE_LCTRL])  game->keys[6] = 1;
    if ( keys[SDL_SCANCODE_TAB])    game->keys[8] = 1;
    if ( keys[SDL_SCANCODE_I])      game->keys[9] = 1;


    if (gamepad)
    {

        SDL_GameControllerUpdate();

        game->gamepadLAxis.x = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTX) / 1000;

        if (game->gamepadLAxis.x > 0 && game->gamepadLAxis.x < GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadLAxis.x = 0;
        }

        if (game->gamepadLAxis.x < 0 && game->gamepadLAxis.x > -GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadLAxis.x = 0;
        }


        game->gamepadLAxis.y = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_LEFTY) / 1000;

        if (game->gamepadLAxis.y > 0 && game->gamepadLAxis.y < GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadLAxis.y = 0;
        }

        if (game->gamepadLAxis.y < 0 && game->gamepadLAxis.y > -GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadLAxis.y = 0;
        }


        game->gamepadRAxis.x = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTX) / 1000;

        if (game->gamepadRAxis.x > 0 && game->gamepadRAxis.x < GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadRAxis.x = 0;
        }

        if (game->gamepadRAxis.x < 0 && game->gamepadRAxis.x > -GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadRAxis.x = 0;
        }


        game->gamepadRAxis.y = SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_RIGHTY) / 1000;

        if (game->gamepadRAxis.y > 0 && game->gamepadRAxis.y < GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadRAxis.y = 0;
        }

        if (game->gamepadRAxis.y < 0 && game->gamepadRAxis.y > -GameProto::GAMEPAD_DEADZONE)
        {
            game->gamepadRAxis.y = 0;
        }


        if (game->doRumble)
        {
            SDL_GameControllerRumble(gamepad, 0x8888, 0x8888, 250);
            game->doRumble = false;
        }

        if (SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 1000 > 0) game->keys[6] = 1;
        if (SDL_GameControllerGetAxis(gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 1000 > 0) game->keys[6] = 1;

        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_A))             game->keys[4] = 1;
        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_B))             game->keys[5] = 1;

        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))  game->keys[7] = 1;
        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) game->keys[7] = 1;

        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))       game->keys[0] = 1;
        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))     game->keys[1] = 1;
        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))    game->keys[2] = 1;
        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))     game->keys[3] = 1;
        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_START))         game->keys[8] = 1;
        if (SDL_GameControllerGetButton(gamepad, SDL_CONTROLLER_BUTTON_X))             game->keys[9] = 1;

    }

}


void disarray::processEventsPc()
{
    SDL_Event event;
    float scaleX = 1.f;
    float scaleY = 1.f;

    while( SDL_PollEvent( &event ) ) 
    {

        switch( event.type ) 
        {

        case SDL_TEXTINPUT:
        {
            strcpy(game->editText, event.text.text);
        } break;

        case SDL_KEYUP:
        {
            game->globalKEY = 0;
            game->globalKeyUp = (char)event.key.keysym.scancode;
        } break;

        case SDL_KEYDOWN:{

            game->globalKEY = (char)event.key.keysym.scancode;
            switch( event.key.keysym.sym ) 
            {
                default:{} break;
                case SDLK_F1: {++game->debugMode; if (game->debugMode > 1) game->debugMode = 0;} 
            }
        } break;
        case SDL_MOUSEBUTTONUP:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            game->touches->up.push_back(pos);
            game->touches->allfingersup = true;
        } break;
        case SDL_MOUSEBUTTONDOWN:{
            Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
            game->touches->down.push_back(pos);
            game->touches->allfingersup = false;

        } break;

        case SDL_MOUSEWHEEL:
        {
            game->keys[7] = 1;
        } break;

        case SDL_MOUSEMOTION:{
            if(SDL_GetMouseState(0, 0)&SDL_BUTTON_LMASK){
                Vector3D pos(event.button.x * scaleX, event.button.y * scaleY, 0);
                game->touches->move.push_back(pos);
                game->touches->allfingersup = false;
            }
        }break;


        case SDL_QUIT:
        {
            game->works = false;
        }break;

        }

    }

}

#endif
