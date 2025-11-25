#include <disarray/disarray.h>
#include <disarray/GameProto.h>
#ifndef __ANDROID__
#include <disarray/SDLVideo.h>
#endif

disarray::disarray(GameProto* ng, _SDL_GameController* controller)
{
    game = nullptr;

    if (ng)
    {
        game = ng;
    }

    gamepad = controller;
}

#ifndef __ANDROID__
void disarray::whileLoopPC(SDLVideo* sdl)
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

        otherWhileLoopActivities();
        SDL_Delay(0.6);
    }

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
#endif