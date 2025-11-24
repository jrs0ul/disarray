#include <vector>
#include <algorithm>
#include <cwchar>
#include "Game.h"

const int boardX = 16;
const int boardY = 445;
const int tileSize = 64;

//--------------------------------------
void Game::loadConfig(){
#ifndef __ANDROID__
    char buf[1024];
    sprintf(buf, "%s/settings.cfg", DocumentPath);
    sys.load(buf);
    ScreenWidth = sys.ScreenWidth;
    ScreenHeight = sys.ScreenHeight;
    windowed = sys.useWindowed;

    sys.write(buf);
#endif

}
//---------------------------------------
void Game::LoadShader(ShaderProgram* shader, const char* name)
{
    shader->create(false);

    char error[1024];
    char buf[512];

    Shader vert;
    Shader frag;

    printf("Loading vertex shader...\n");
    sprintf(buf, "shaders/%s.vert", name);
#ifdef __ANDROID__
    vert.loadGL(VERTEX_SHADER, buf, AssetManager);
#else
    vert.loadGL(VERTEX_SHADER, buf);
#endif

    printf("Loading fragment shader...\n");
    sprintf(buf, "shaders/%s.frag", name);
#ifdef __ANDROID__
    frag.loadGL(FRAGMENT_SHADER, buf, AssetManager);
#else
    frag.loadGL(FRAGMENT_SHADER, buf);
#endif

    shader->attach(vert);
    shader->attach(frag);
    shader->link();

    shader->getLog(error, 1024);
    if (strlen(error)) {
#ifdef __ANDROID__
        LOGI("--%s--", buf);
        LOGI("%s", error);
    }
    LOGI("---------------");
#else
        printf("--%s--\n", buf);
        puts(error);
    }
    puts("-----------");
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
void Game::init(){

    srand(time(0));

#ifndef __ANDROID__
    LoadExtensions();
#endif
    printf("Creating shaders...\n");
   
    LoadShader(&defaultShader, "default");
    LoadShader(&colorShader, "justcolor");

    colorShader.use();

    glClearColor(0.5f, 0.5f, 0.6f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);

    glEnable (GL_BLEND);


    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    MatrixOrtho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, -400, 400, OrthoMatrix);

    SoundSystem* ss = SoundSystem::getInstance();
    ss->init(0);
#ifndef __ANDROID__
    ss->loadFiles("sfx", "list.xml");
    pics.load("pics/imagesToLoad.xml");
    music.open("music/music.ogg");
#else
    ss->loadFiles("sfx", "list.xml", AssetManager);
    pics.load("pics/imagesToLoad.xml", AssetManager);
    music.open("music/music.ogg", AssetManager);
#endif
    ss->setVolume(0, sys.soundFXVolume);
    ss->setupListener(Vector3D(0, 0, 0).v, Vector3D(0, 0, 0).v);

    
    music.setVolume(sys.musicVolume);
    music.playback();


    useAccel = false;

    gameMode = TITLE;
    lavaSpeed = 100;
    lives = 3;
    score = 0;
    restartGame();
}

//-------------------
void Game::render(){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int MatrixID = defaultShader.getUniformID("ModelViewProjection");
    FlatMatrix identity;
    MatrixIdentity(identity.m);

    int MatrixID2 = colorShader.getUniformID("ModelViewProjection");
    modelMatrixId = modelShader.getUniformID("ModelViewProjection");

    FlatMatrix finalM = identity * OrthoMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, finalM.m);
    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, finalM.m);


    Render2D();
    pics.drawBatch(&colorShader, &defaultShader, 666);
}

//---------------------
void Game::logic(){
#ifndef __ANDROID__
    if (music.playing())
        music.update();
#endif

    GameLoop();
    
    touches.oldDown.clear();
    for (unsigned long i = 0; i < touches.down.size(); i++ ){
        Vector3D v = touches.down[i];
        touches.oldDown.push_back(v);
    }
    touches.up.clear();
    touches.down.clear();
    touches.move.clear();
}
//-------------------------
void Game::destroy(){

    music.release();
    SoundSystem* ss = SoundSystem::getInstance();
    ss->freeData();
    ss->exit();

    pics.destroy();

    defaultShader.destroy();
}

//==================================================
int Game::FPS (void)
{
    static int ctime = 0, FPS = 0, frames = 0, frames0 = 0;

    if ((int)TimeTicks >= ctime) 
    {
        FPS = frames - frames0;
        ctime = (int)TimeTicks + 1000;
        frames0 = frames;
    }

    frames = frames+1;
    return FPS;
}


//---------------------------
void Game::onBack()
{

}

//------------------------------------
void Game::GameLoop()
{
    if (gameMap[playerY / 8 * MAPWIDTH + playerX / 8] == 9)
    {
        lavaSpeed = 100;
        
        --lives;
        if (lives == 0)
        {
            score = 0;
            lives = 3;
        }
        SoundSystem::getInstance()->playsound(0);
        restartGame();
        return;
    }


    if (!isPlayerMining)
    {

        if (Keys[2] == 1)
        {
            if (playerX > 4 && gameMap[playerY / 8 * MAPWIDTH + ((playerX - 5) / 8)] != 7 &&
                    gameMap[playerY / 8 * MAPWIDTH + ((playerX - 5) / 8)] != 8 && (playerY - 4) % 8 == 0)
            {
                playerX -= 1; 
            }

            flipPlayer = true;
            ++playerFrameTimer;
        }

        if (Keys[3] == 1)
        {
            if (playerX < MAPWIDTH * 8 - 4 && gameMap[playerY / 8 * MAPWIDTH + ((playerX + 4) / 8)] != 7 &&
                    gameMap[playerY / 8 * MAPWIDTH + ((playerX + 4) / 8)] != 8 && (playerY - 4) % 8 == 0)
            {
                playerX += 1;

                if (gameMap[playerY / 8 * MAPWIDTH + playerX / 8] == 5)
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

        if (Keys[1] == 1 && gameMap[((playerY + 4) / 8) * MAPWIDTH + playerX / 8] == 11)
        {
            playerY += 1;
            playerX = (playerX / 8) * 8 + 4;
            playerFrame = 3;
        }

    }

    
    if (playerFrameTimer >= 5)
    {
        playerFrameTimer = 0;
        playerFrame = (playerFrame == 1) ? 2 : 1;
        
    }

    if (Keys[4] == 1 && !isPlayerMining)
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

            uint8_t brickXToMine = (flipPlayer == false) ? (playerX + 5) / 8 : (playerX - 5) / 8;
            uint8_t brickYToMine = playerY / 8;

            if (brickYToMine < MAPHEIGHT && brickXToMine < MAPWIDTH)
            {
                if (gameMap[brickYToMine * MAPWIDTH + brickXToMine] != 9 && gameMap[brickYToMine * MAPWIDTH + brickXToMine] != 0)
                {
                    gameMap[brickYToMine * MAPWIDTH + brickXToMine] = (rand() % 10 == 9) ? 8 : 0;

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
    if (playerY / 8 > 1 && !runLava)
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
    for (uint8_t i = 0; i < 13; ++i)
    {
        for (uint8_t j = 0; j < 7; ++j)
        {
            pics.draw(1, i * 40, j * 40, gameMap[j * MAPWIDTH + i], false, 5.f, 5.f);
        }
    }

    int scaleX = 5;

    if (flipPlayer)
    {
        scaleX = -5;
    }

    pics.draw(1, playerX * 5, playerY * 5, playerFrame, true, scaleX, 5.f);
    DrawDebugText();
}

//----------------------------
void Game::DrawDebugText(){

    char buf[256];
    sprintf(buf, "FPS:%d", FPS());
    WriteText(580, 2, pics, 0, buf, 0.8f, 0.8f);
    sprintf(buf, "lavaPosX %u", lavaPosX);
    WriteText(520, 20, pics, 0, buf, 0.8f, 0.8f);
    sprintf(buf, "lavaPosy %u", lavaPosY);
    WriteText(520, 40, pics, 0, buf, 0.8f, 0.8f);
    sprintf(buf, "lavaCanGoRight %d", lavaCanGoRight);
    WriteText(520, 60, pics, 0, buf, 0.8f, 0.8f);
    sprintf(buf, "lavaSpeed %u", lavaSpeed);
    WriteText(520, 80, pics, 0, buf, 0.8f, 0.8f);
    sprintf(buf, "lives %u", lives);
    WriteText(520, 120, pics, 0, buf, 0.8f, 0.8f);
    sprintf(buf, "score %u", score);
    WriteText(520, 140, pics, 0, buf, 0.8f, 0.8f);
}
