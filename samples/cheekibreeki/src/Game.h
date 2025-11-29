#pragma once

#include <ctime>

#include <disarray/audio/SoundSystem.h>
#include <disarray/audio/OggStream.h>
#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#endif
#include <disarray/SpriteBatcher.h>
#include <disarray/Vectors.h>
#include <disarray/TouchData.h>
#include <disarray/gui/Text.h>
#include <disarray/GameProto.h>
//#include "gui/Button.h"
//#include "SysConfig.h"
//#include "Utils.h"
//#include "Usefull.h"
//#include <disarray/Entity.h>
#include <disarray/ShaderProgram.h>
#include <disarray/rm2.h>
#include "Gopnik.h"



#ifndef __IPHONEOS__
//   #define _DEBUG_ 1
#endif


const float posX = 45;
const float posY = 5;
const float fieldWidth = 3.f;
const float fieldHeight = 2.f;
const float scale = 100.f;
const float TouchScreenJoystickRadius = 45.f;
const float TouchScreenJoystickX = 90;
const float TouchScreenJoystickY = 400;


enum GameModes
{
    TITLE,
    GAME
};

struct ChainElement
{
    int x;
    int y;
    int value;
};




//============================================
class Game : public GameProto
{

public:
   
    Vector3D oldMoveTouch;

    void Render2D();
    void GameLoop();

    void DrawDebugText();
    
    Game()
    {
    }
    
    void init(bool useVulkan) override;
    void loadConfig();
    void logic();
    void render();
    void destroy();
    
private:

    void restartGame();
    void addTileToChainIfTouched(Vector3D& touch);
    void gravity();

    void OnTitle();
    void OnGame();

    void onChainFinish();

    void DrawTitle();
    void DrawGame();
    void DrawChain();

public:
    GameModes           gameMode;
private:

    float                ViewMatrix[16];
    FlatMatrix           pushed;


    int                  board[6][7];
    DArray<ChainElement> chain;
    SubsetCollection     model;

    int                  modelMatrixId;


    Gopnik               player;
    Gopnik               enemy;

    DArray<Animation>    animations;


    float                destructionProgress;
    float                tileFallingProgress;

    int                  playerDidDamage;

    
    bool                 chainDestroyed;
    bool                 tilesFall;


    bool                 match3InputBlocked;
    
    bool                 movePlayerForward;
    bool                 movePlayerBackward;
    bool                 moveEnemyForward;
    bool                 moveEnemyBackward;
};


