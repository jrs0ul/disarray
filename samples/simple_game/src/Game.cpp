#include "Game.h"
#include <disarray/audio/OggStream.h>
#include <disarray/audio/SoundSystem.h>
#include <disarray/SpriteBatcher.h>
#include <disarray/ShaderLoader.h>
#include <disarray/SysConfig.h>
#include <disarray/TouchData.h>
#include <disarray/gui/Text.h>

void Game::loadConfig()
{
#ifndef __ANDROID__
    char buf[1024];
    sprintf(buf, "%s/settings.cfg", documentPath);
    if (!sys->load(buf))
    {
        sys->ScreenWidth = 640;
        sys->ScreenHeight = 360;
        sys->useWindowed = true;
    }
    screenWidth = sys->ScreenWidth;
    screenHeight = sys->ScreenHeight;
    windowed = sys->useWindowed;

    sys->write(buf);
#endif
}

void Game::restartGame()
{
    initMap();

    playerX = 4;
    playerY = 4;
    playerFrame = 1;
    flipPlayer = false;
    playerFrameTimer = 0;
    isPlayerMining = false;
    playerMineTimer = 0;
    lavaPosX = 0;
    lavaPosY = 0;
    runLava = false;
    lavaTimer = 0;
    lavaCanGoRight = true;
}


void Game::initMap()
{
    uint8_t ladderX = 255;

    for (uint8_t j = 1; j < MAPHEIGHT; ++j)
    {
        uint8_t tmp = rand() % MAPWIDTH;

        while (tmp == ladderX)
        {
            tmp = rand() % MAPWIDTH;
        }

        ladderX = tmp;

        for (uint8_t i = 0; i < MAPWIDTH; ++i)
        {
            if ( i == ladderX)
            {
                gameMap[j * MAPWIDTH + i] = 11;
            }
            else
            {
                gameMap[j * MAPWIDTH + i] = 7;
            }
        }
    }

    gameMap[0] = 0;
    gameMap[1] = 0;

    for (uint8_t i = 2; i < MAPWIDTH; ++i)
    {
        gameMap[i] = 7;
    }

}

//----------------------------------------
void Game::init(bool useVulkan)
{
    srand(time(0));
    LoadExtensions();

    shaders->init(useVulkan, vk, pics);
    if (!shaders->load("shaders", "list.xml"))
    {
        printf("NO SHADERS!\n");
    }

    shaders->shaders[1].use();

    glClearColor(0.5f, 0.5f, 0.6f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);
    glEnable (GL_BLEND);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, -400, 400, orthoMatrix);

    SoundSystem* ss = SoundSystem::getInstance();
    ss->init(0);

    if (!ss->loadFiles("sfx/", "list.xml"))
    {
        printf("SFXs ARE MISSING!\n");
    }

    pics->load("pics/imagesToLoad.xml");
    music->open("music/music.ogg");
    ss->setVolume(0, 100);
    ss->setupListener(Vector3D(0, 0, 0).v, Vector3D(0, 0, 0).v);

    music->setVolume(sys->musicVolume);
    music->playback();

    gameMode = TITLE;
    lavaSpeed = 100;
    lives = 3;
    score = 0;
    restartGame();
}

//-------------------
void Game::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int MatrixID = shaders->shaders[0].getUniformID("ModelViewProjection");
    FlatMatrix identity;
    MatrixIdentity(identity.m);

    int MatrixID2 = shaders->shaders[1].getUniformID("ModelViewProjection");

    FlatMatrix finalM = identity * orthoMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, finalM.m);
    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, finalM.m);

    Render2D();
    pics->drawBatch(&shaders->shaders[1], &shaders->shaders[0], 666);
}

//---------------------
void Game::logic()
{
    if (music->playing())
    {
        music->update();
    }

    GameLoop();

    touches->oldDown.clear();
    touches->oldDown = touches->down;
    touches->up.clear();
    touches->down.clear();
    touches->move.clear();
}
//-------------------------
void Game::destroy()
{
    GameProto::destroy();
}

//------------------------------------
void Game::GameLoop()
{
    if (gameMap[playerY / TILE_SIZE * MAPWIDTH + playerX / TILE_SIZE] == 9)
    {
        lavaSpeed = 100;

        --lives;
        if (lives == 0)
        {
            score = 0;
            lives = 3;
        }
        SoundSystem::getInstance()->playsound(0);
        doRumble = true;
        restartGame();
        return;
    }

    if (!isPlayerMining)
    {

        if (keys[3] == 1)
        {
            if (playerX > 4 && gameMap[playerY / TILE_SIZE * MAPWIDTH + ((playerX - 5) / TILE_SIZE)] != 7 &&
                    gameMap[playerY / TILE_SIZE * MAPWIDTH + ((playerX - 5) / TILE_SIZE)] != 8 && (playerY - 4) % 8 == 0)
            {
                playerX -= 1; 
            }

            flipPlayer = true;
            ++playerFrameTimer;
        }

        if (keys[2] == 1)
        {
            if (playerX < MAPWIDTH * TILE_SIZE - 4 && gameMap[playerY / 8 * MAPWIDTH + ((playerX + 4) / TILE_SIZE)] != 7 &&
                    gameMap[playerY / TILE_SIZE * MAPWIDTH + ((playerX + 4) / TILE_SIZE)] != 8 && (playerY - 4) % TILE_SIZE == 0)
            {
                playerX += 1;

                if (gameMap[playerY / TILE_SIZE * MAPWIDTH + playerX / TILE_SIZE] == 5)
                {
                    // Increase lava flow
                    lavaSpeed -= 5;

                    if (lavaSpeed < 10)
                    {
                        lavaSpeed = 10;
                    }

                    restartGame();
                    return;
                }
            }

            flipPlayer = false;
            ++playerFrameTimer;
        }

        if (keys[1] == 1 && gameMap[((playerY + 4) / TILE_SIZE) * MAPWIDTH + playerX / TILE_SIZE] == 11)
        {
            playerY += 1;
            playerX = (playerX / TILE_SIZE) * TILE_SIZE + 4;
            playerFrame = 3;
        }

    }

    if (playerFrameTimer >= 5)
    {
        playerFrameTimer = 0;
        playerFrame = (playerFrame == 1) ? 2 : 1;
    }

    if (keys[4] == 1 && !isPlayerMining)
    {
        isPlayerMining = true;
        playerMineTimer = 0;
        playerFrame = 4;
    }

    if (isPlayerMining)
    {
        ++playerMineTimer;

        if (playerMineTimer > 30)
        {
            isPlayerMining = false;
            playerFrame = 2;
            playerMineTimer = 0;

            uint8_t brickXToMine = (flipPlayer == false) ? (playerX + 5) / TILE_SIZE : (playerX - 5) / TILE_SIZE;
            uint8_t brickYToMine = playerY / TILE_SIZE;

            if (brickYToMine < MAPHEIGHT && brickXToMine < MAPWIDTH)
            {
                if (gameMap[brickYToMine * MAPWIDTH + brickXToMine] != 9 && gameMap[brickYToMine * MAPWIDTH + brickXToMine] != 0)
                {
                    gameMap[brickYToMine * MAPWIDTH + brickXToMine] = (rand() % 10 == 9) ? TILE_SIZE : 0;

                    if (gameMap[brickYToMine * MAPWIDTH + brickXToMine] == 0)
                    {
                        score += 2;
                    }

                    if (brickYToMine * MAPWIDTH + brickXToMine == MAPWIDTH * MAPHEIGHT - 1)
                    {
                        gameMap[brickYToMine * MAPWIDTH + brickXToMine] = 5;
                    }
                }
            }
        }
    }
 //----------- LAVA stuff
    if (playerY / TILE_SIZE > 1 && !runLava)
    {
        runLava = true;
        gameMap[0] = 9;
    }

    if (runLava)
    {
        ++lavaTimer;

        if (lavaTimer > lavaSpeed)
        {
            lavaTimer = 0;

            if ((gameMap[(lavaPosY + 1) * MAPWIDTH + lavaPosX] == 0 || gameMap[(lavaPosY + 1) * MAPWIDTH + lavaPosX] == 11
                 || gameMap[(lavaPosY + 1) * MAPWIDTH + lavaPosX] == 9) && lavaPosY < MAPHEIGHT - 1)
            {
                ++lavaPosY;
                lavaTurnX = lavaPosX;
                lavaCanGoRight = true;
            }
            else if ((gameMap[lavaPosY * MAPWIDTH + lavaPosX + 1] == 0 || gameMap[lavaPosY * MAPWIDTH + lavaPosX + 1] == 11
                    || gameMap[lavaPosY * MAPWIDTH + lavaPosX + 1] == 9) && lavaPosX < MAPWIDTH - 1  && lavaCanGoRight)
            {
                ++lavaPosX;
            }
            else {

                if (lavaCanGoRight)
                {
                    lavaCanGoRight = false;
                    lavaPosX = lavaTurnX;
                }

                if (gameMap[lavaPosY * MAPWIDTH + lavaPosX - 1] == 0 || gameMap[lavaPosY * MAPWIDTH + lavaPosX - 1] == 11
                    || gameMap[lavaPosY * MAPWIDTH + lavaPosX - 1] == 9)
                {
                    if (lavaPosX > 0)
                    {
                        --lavaPosX;
                    }
                }
            }


            if (lavaPosY * MAPWIDTH + lavaPosX < MAPWIDTH * MAPHEIGHT)
            {
                gameMap[lavaPosY * MAPWIDTH + lavaPosX] = 9;
            }
        }
    }
}

//---------------------------
void Game::Render2D()
{
    for (uint8_t i = 0; i < MAPWIDTH; ++i)
    {
        for (uint8_t j = 0; j < MAPHEIGHT; ++j)
        {
            pics->draw(1, i * 40, j * 40, gameMap[j * MAPWIDTH + i], false, (float)TILE_SCALE, (float)TILE_SCALE);
        }
    }

    float scaleX = (float)((flipPlayer) ? -TILE_SCALE : TILE_SCALE);

    pics->draw(1, playerX * TILE_SCALE, playerY * TILE_SCALE, playerFrame, true, scaleX, (float)TILE_SCALE);
    DrawDebugText();
}

//----------------------------
void Game::DrawDebugText()
{
    char buf[256];
    sprintf(buf, "FPS:%d", fps());
    WriteText(580, 2, *pics, 0, buf, 0.8f, 0.8f);
    sprintf(buf, "lives %u", lives);
    WriteText(520, 120, *pics, 0, buf);//, 1.0f, 0.8f);
    sprintf(buf, "score %u", score);
    WriteText(520, 140, *pics, 0, buf);//, 0.8f, 0.8f);
}
