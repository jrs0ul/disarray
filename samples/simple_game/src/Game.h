#pragma once

#include <disarray/GameProto.h>
#include <disarray/Vectors.h>
#include <disarray/Matrix.h>
#include <disarray/Useful.h>


class Game : public GameProto
{

public:
    unsigned globalKEY;

    void gameLoop();

    Game(){}

    void init(bool useVulkan) override;
    void loadConfig(const char* path) override;
    void logic() override;
    void render() override;

private:
    void restartGame();
    void initMap();

private:
    static const uint8_t        MAPWIDTH = 13;
    static const uint8_t        MAPHEIGHT = 7;
    static const uint8_t        TILE_SCALE = 5;
    static const uint8_t        TILE_SIZE = 8;

    uint8_t                     gameMap[MAPWIDTH * MAPHEIGHT];
    uint16_t                    score;
    uint8_t                     playerX;
    uint8_t                     playerY;
    uint8_t                     playerFrame;
    uint8_t                     playerFrameTimer;
    uint8_t                     playerMineTimer;
    uint8_t                     lavaPosX;
    uint8_t                     lavaPosY;
    uint8_t                     lavaTurnX;
    uint8_t                     lavaTimer;
    uint8_t                     lavaSpeed;
    uint8_t                     lives;
    bool                        flipPlayer;
    bool                        isPlayerMining;
    bool                        runLava;
    bool                        lavaCanGoRight;
};
