#pragma once

#include <ctime>
#include <disarray/GameProto.h>
#include <disarray/audio/SoundSystem.h>
#include <disarray/audio/OggStream.h>
#include <disarray/SpriteBatcher.h>
#include <disarray/Vectors.h>
#include <disarray/TouchData.h>
#include <disarray/gui/Text.h>
#include <disarray/SysConfig.h>
#include <disarray/Matrix.h>
#include <disarray/Useful.h>
#include <disarray/ShaderProgram.h>




#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480



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


//============================================
class Game : public GameProto
{

public:
  
#ifdef __ANDROID__
    AAssetManager* AssetManager;
#endif



    int ScreenWidth;
    int ScreenHeight;
    bool windowed;

    char DocumentPath[512];

    unsigned char Keys[20];
    unsigned char OldKeys[20];
    Vector3D gamepad;

    OggStream music;
    
    SpriteBatcher pics;
    
    long TimeTicks;
    float DeltaTime;
    float Accumulator;
    float DT;
    long tick;


    
    int MouseXOld, MouseYOld, MouseX, MouseY, MouseButton, MouseButtonOld;
    
    
    Vector3D oldMoveTouch;
    
    char Key, OldKey;
    
    
    unsigned globalKEY;
    
    
    
    bool useAccel;
    Vector3D _acceleration;
    Vector3D firstTouch;
    Vector3D pos;

    
    SystemConfig             sys;
    
    TouchData                touches;
    bool                     Works;
    int                      DebugMode;

    void Render2D();
    void GameLoop();

    void DrawDebugText();
    int  FPS();
    
    Game()
    {

        ScreenWidth = 640;
        ScreenHeight = 480;
        windowed = false;

        Accumulator = 0;
        DT = 1000.0f/60.0f/1000.0f;
        oldMoveTouch = Vector3D(-1, -1, 0);
        Works = true;
        tick = 0;
        TimeTicks = 0;

        DebugMode = 0;
    }
    
    void init();
    void loadConfig();
    void logic();
    void render();
    void destroy();
    void onBack();
    
private:

    void LoadShader(ShaderProgram* shader, const char* name);



    void restartGame();

    void initMap();



public:
    GameModes           gameMode;
private:

    float                OrthoMatrix[16];
    float                ViewMatrix[16];
    FlatMatrix           pushed;
    int                  modelMatrixId;


    ShaderProgram        defaultShader;
    ShaderProgram        colorShader;
    ShaderProgram        modelShader;



    static const uint8_t        MAPWIDTH = 13;
    static const uint8_t        MAPHEIGHT = 7;
    
    uint8_t                     gameMap[MAPWIDTH * MAPHEIGHT];
    uint8_t                     playerX;
    uint8_t                     playerY;
    uint8_t                     playerFrame;
    bool                        flipPlayer;
    bool                        isPlayerMining;
    uint8_t                     playerFrameTimer;
    uint8_t                     playerMineTimer;
    uint8_t                     lavaPosX;
    uint8_t                     lavaPosY;
    uint8_t                     lavaTurnX;
    bool                        runLava;
    bool                        lavaCanGoRight;
    uint8_t                     lavaTimer;
    uint8_t                     lavaSpeed;
    uint16_t                    score;
    uint8_t                     lives;

};


