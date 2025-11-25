#pragma once

#include <disarray/GameProto.h>
#include <disarray/Vectors.h>
#include <disarray/Matrix.h>
#include <disarray/Useful.h>

const float posX = 45;
const float posY = 5;
const float fieldWidth = 3.f;
const float fieldHeight = 2.f;
const float scale = 100.f;


enum GameModes
{
    TITLE,
    GAME
};


//============================================
class Game : public GameProto
{

public:
    bool windowed;
    unsigned char Keys[20];
    unsigned char OldKeys[20];
    Vector3D gamepad;

    int MouseXOld, MouseYOld, MouseX, MouseY, MouseButton, MouseButtonOld;

    Vector3D oldMoveTouch;
    char Key, OldKey;

    unsigned globalKEY;

    bool useAccel;
    Vector3D _acceleration;
    Vector3D firstTouch;
    Vector3D pos;

    int      DebugMode;

    void Render2D();
    void GameLoop();
    void DrawDebugText();

    Game()
    {
        windowed = false;
        oldMoveTouch = Vector3D(-1, -1, 0);
        DebugMode = 0;
    }

    void init();
    void loadConfig();
    void logic();
    void render();
    void destroy() override;
    void onBack();

private:
    void restartGame();
    void initMap();

public:
    GameModes           gameMode;
private:

    FlatMatrix           pushed;
    int                  modelMatrixId;

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
