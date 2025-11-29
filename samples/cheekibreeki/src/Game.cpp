#include "Game.h"
#include <vector>
#include <algorithm>
#include <disarray/ShaderLoader.h>
#include <disarray/SysConfig.h>
//#include <cwchar>


const int boardX = 16;
const int boardY = 445;
const int tileSize = 64;

void Game::restartGame()
{
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            board[i][j] = rand() % 4 + 1;
        }
    }
    

     chain.destroy();

     player.x = 100.f;
     player.y = 280.f;

     playerDidDamage = 0;

     enemy.x = 380.f;
     enemy.y = 280.f;

     player.maxhealth = 100;
     player.health = player.maxhealth;
     player.nextAnimation = 0;
     player.currentAnimation = 0;
     
     enemy.maxhealth = 50;
     enemy.health = enemy.maxhealth;
     enemy.nextAnimation = 0;
     enemy.currentAnimation = 0;
    
     match3InputBlocked = false;

     movePlayerForward = false;
     movePlayerBackward = false;

     moveEnemyForward = false;
     moveEnemyBackward = false;

     chainDestroyed = false;
     destructionProgress = 0.f;
}


void Game::addTileToChainIfTouched(Vector3D& touch)
{

   
    if (touch.x > boardX && touch.y > boardY &&
            touch.x < boardX + 7 * tileSize && touch.y < boardY + 6 * tileSize)
    {
        ChainElement e;
        e.x = (touch.x - boardX) / tileSize;
        e.y = (touch.y - boardY) / tileSize;
        e.value = board[e.y][e.x];

        bool found = false;

        for (int i = 0; i < (int)chain.count(); ++i)
        {
            if (e.x == chain[i].x && e.y == chain[i].y && e.value == chain[i].value)
            {
                found = true;
                break;
            }
        }

        bool matchesChainValue = true;

        if (chain.count())
        {
            int lastValue = chain[chain.count() - 1].value;

            if (lastValue != e.value)
            {
                matchesChainValue = false;
            }
        }


        bool isAdjacent = false;



        if (chain.count())
        {
            for (int i = -1; i < 2; ++i)
            {
                for (int j = -1; j < 2; ++j)
                {
                    if ((chain[chain.count() - 1].x == e.x + j) &&
                            (chain[chain.count() - 1].y == e.y + i))
                    {
                        isAdjacent = true;
                    }
                }
            }
        }
        else
        {
            isAdjacent = true;
        }

        if (found && isAdjacent && (chain[chain.count() - 1].x != e.x || chain[chain.count() - 1].y != e.y))
        {
            chain.remove(chain.count() - 1);
            return;
        }



        if (!found && matchesChainValue && isAdjacent)
        {
            chain.add(e);
        }
    }

}


void Game::gravity()
{

    for (int h = 4; h >= 0; --h)
    {
        for (int i = 0; i < 7; ++i)
        {
            if (board[h + 1][i] == 0 && board[h][i])
            {
                board[h + 1][i] = board[h][i];
                board[h][i] = 0;
            }

            //spawn

            if (h == 0 && board[h][i] == 0)
            {
                board[h][i] = rand() % 4 + 1;
            }
        }
    }
}


void Game::OnTitle()
{

    if (touches->up.size())
    {
        restartGame();
        gameMode = GAME;

    }

}

void Game::OnGame()
{

    if (touches->down.size())
    {
        if (!match3InputBlocked)
        {
            addTileToChainIfTouched(touches->down[0]);
        }
    }

    if (touches->move.size())
    {
        if (!match3InputBlocked)
        {
            addTileToChainIfTouched(touches->move[0]);
        }
    }

    if (touches->up.size())
    {
        if (!match3InputBlocked)
        {
            if (chain.count() >= 3)
            {
                onChainFinish();
            }

            chain.destroy();

        }

    }

    if (chainDestroyed)
    {
        destructionProgress += deltaTime;
        if (destructionProgress >= 1.f)
        {
            chainDestroyed = false;
            destructionProgress = 0.f;
        }
    }
    else
    {
        if (!tilesFall)
        {
            gravity();
            tilesFall = true;
        }

        tileFallingProgress += deltaTime;
        if (tileFallingProgress >= 0.5f)
        {
            tileFallingProgress = 0.f;
            tilesFall = false;
        }
    }

    if (movePlayerForward)
    {
        player.x += 4;
        if (player.x > 250)
        {
            player.x = 250;
            movePlayerForward = false;
            player.nextAnimation = 1;
            match3InputBlocked = true;
            //attackEnemy();
        }
    }

    if (movePlayerBackward)
    {
        player.x -= 4;
        if (player.x <= 100)
        {
            movePlayerBackward = false;
            //attackPlayer();
            moveEnemyForward = true;
        }
    }

    if (moveEnemyForward)
    {
        enemy.x -= 4;
        if (enemy.x <= 230)
        {
            //player.x = 250;
            moveEnemyForward = false;
            enemy.nextAnimation = 1;
            //attackPlayer();
        }
    }

    if (moveEnemyBackward)
    {
        enemy.x += 4;
        if (enemy.x >= 380)
        {
            moveEnemyBackward = false;
            match3InputBlocked = false;
        }
    }


    player.animate(animations);
    enemy.animate(animations);

    if (player.currentAnimation == 1 && player.animFrame == animations[1].endFrame)
    {
        enemy.health -= playerDidDamage;

        if (enemy.health <= 0)
        {
            enemy.nextAnimation = 2; //die
        }

        movePlayerBackward = true;
    }

    if (enemy.currentAnimation == 1 && enemy.animFrame == animations[1].endFrame)
    {
        player.health -= 30;

        if (player.health < 0)
        {
            player.nextAnimation = 2; //die
        }

        moveEnemyBackward = true;
    }

    if (enemy.currentAnimation == 2 && enemy.animFrame == animations[2].endFrame)
    {
        gameMode = TITLE;
    }

    if (player.currentAnimation == 2 && player.animFrame == animations[2].endFrame)
    {
        gameMode = TITLE;
    }

}

void Game::onChainFinish()
{
    for (int i = 0; i < (int)chain.count(); ++i)
    {
        board[chain[i].y][chain[i].x] = 0;
    }

    chainDestroyed = true;

    if (chain[0].value > 2) //weapon tile
    {
        /**/
        movePlayerForward = true;
        playerDidDamage = 2 * chain.count();
    }
    else
    {

        match3InputBlocked = true;
        player.health += chain.count() * 10;
        if (player.health > player.maxhealth)
        {
            player.health = player.maxhealth;
        }
        printf("healed by %d\n", (int)chain.count() * 10 );

        //attackPlayer();
        moveEnemyForward = true;
    }

}


void Game::DrawTitle()
{
    pics->draw(4, 240, 425, 0, true, 0.8f);
}

void Game::DrawGame()
{

    pics->draw(6, 0, 0, 0);

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            pics->draw(5, boardX + tileSize * j, boardY + tileSize * i, 0);

        }
    }

    pics->drawBatch(&shaders->shaders[1], &shaders->shaders[0], 666);
    DrawChain();

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 7; ++j)
        {
            
            if (board[i][j])
            {
                pics->draw(5, boardX + tileSize * j, boardY + tileSize * i, board[i][j]);
            }

        }
    }    
   

    pics->drawBatch(&shaders->shaders[1], &shaders->shaders[0], 666);


    shaders->shaders[2].use();
    glEnable(GL_DEPTH_TEST);

    player.draw(false, pics->getGLName(7), model, shaders->shaders[2], modelMatrixId, orthoMatrix);
    enemy.draw(true, pics->getGLName(7), model, shaders->shaders[2], modelMatrixId, orthoMatrix);

    glDisable(GL_DEPTH_TEST);
    
    shaders->shaders[1].use();

    player.drawHudBars(*pics);
    enemy.drawHudBars(*pics);
    pics->drawBatch(&shaders->shaders[1], &shaders->shaders[0], 666);

    
}
//-------------------------------------
void Game::DrawChain()
{

    glDisable(GL_TEXTURE_2D);
    shaders->shaders[1].use();

    float verts[chain.count() * 12];
    float colors[chain.count() * 4 * 6];

    for (int i = 1; i < (int)chain.count(); ++i)
    {

        int halfSize = 8;

        if (chain[i - 1].y != chain[i].y && chain[i - 1].x != chain[i].x)
        {
            halfSize = 12;
        }

        if (chain[i - 1].y == chain[i].y)
        {
            verts[i * 12] = boardX + chain[i - 1].x * tileSize + 32;
            verts[i * 12 + 1] = boardY + chain[i - 1].y * tileSize + 32 - halfSize;

            verts[i * 12 + 2] = boardX + chain[i].x * tileSize + 32;
            verts[i * 12 + 3] = boardY + chain[i].y * tileSize + 32 - halfSize;

            verts[i * 12 + 4] = verts[i * 12 + 2];
            verts[i * 12 + 5] = boardY + chain[i].y * tileSize + 32 + halfSize;

            verts[i * 12 + 6] = verts[i * 12];
            verts[i * 12 + 7] = verts[i * 12 + 1];

            verts[i * 12 + 8] = verts[i * 12 + 4];
            verts[i * 12 + 9] = verts[i * 12 + 5];

            verts[i * 12 + 10] = verts[i * 12];
            verts[i * 12 + 11] = boardY + chain[i - 1].y * tileSize + 32 + halfSize; 
        }
        else
        {
            verts[i * 12] = boardX + chain[i - 1].x * tileSize + 32 - halfSize;
            verts[i * 12 + 1] = boardY + chain[i - 1].y * tileSize + 32;

            verts[i * 12 + 2] = boardX + chain[i - 1].x * tileSize + 32 + halfSize;
            verts[i * 12 + 3] = boardY + chain[i - 1].y * tileSize + 32;

            verts[i * 12 + 4] = boardX + chain[i].x * tileSize + 32 + halfSize;
            verts[i * 12 + 5] = boardY + chain[i].y * tileSize + 32 ;

            verts[i * 12 + 6] = verts[i * 12];
            verts[i * 12 + 7] = verts[i * 12 + 1];

            verts[i * 12 + 8] = boardX + chain[i].x * tileSize + 32 - halfSize;
            verts[i * 12 + 9] = boardY + chain[i].y * tileSize + 32;

            verts[i * 12 + 10] = verts[i * 12 + 4];
            verts[i * 12 + 11] = verts[i * 12 + 5]; 

        }

        colors[i * 24] = 1.f;
        colors[i * 24 + 1] = 0.f;
        colors[i * 24 + 2] = 0.f;
        colors[i * 24 + 3] = 1.f;

        colors[i * 24 + 4] = 1.f;
        colors[i * 24 + 5] = 0.f;
        colors[i * 24 + 6] = 0.f;
        colors[i * 24 + 7] = 1.f;

        colors[i * 24 + 8] = 1.f;
        colors[i * 24 + 9] = 0.f;
        colors[i * 24 + 10] = 0.f;
        colors[i * 24 + 11] = 1.f;

        colors[i * 24 + 12] = 1.f;
        colors[i * 24 + 13] = 0.f;
        colors[i * 24 + 14] = 0.f;
        colors[i * 24 + 15] = 1.f;

        colors[i * 24 + 16] = 1.f;
        colors[i * 24 + 17] = 0.f;
        colors[i * 24 + 18] = 0.f;
        colors[i * 24 + 19] = 1.f;

        colors[i * 24 + 20] = 1.f;
        colors[i * 24 + 21] = 0.f;
        colors[i * 24 + 22] = 0.f;
        colors[i * 24 + 23] = 1.f;

    }

    int attribID = shaders->shaders[1].getAttributeID("aPosition"); 
    int ColorAttribID = shaders->shaders[1].getAttributeID("aColor");

    glVertexAttribPointer(attribID, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glEnableVertexAttribArray(attribID);
       
    glVertexAttribPointer(ColorAttribID, 4, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(ColorAttribID);

    glDrawArrays(GL_TRIANGLES, 0, chain.count() * 6);


    glDisableVertexAttribArray(ColorAttribID);
    glDisableVertexAttribArray(attribID);
    glEnable(GL_TEXTURE_2D);
}

//----------------------------------------
void Game::init(bool useVulkan)
{

    srand(time(0));

#ifndef __ANDROID__
    LoadExtensions();
#endif

    #ifdef __ANDROID__
    pics->load("pics/imagesToLoad.xml", androidAssetManager, useVulkan);
#else
    pics->load("pics/imagesToLoad.xml", useVulkan);
#endif

    SoundSystem::getInstance()->init(0);
#ifdef __ANDROID__
    SoundSystem::getInstance()->loadFiles("sfx/", "list.xml", androidAssetManager);
#else
    SoundSystem::getInstance()->loadFiles("sfx/", "list.xml");
#endif

   
    shaders->init(useVulkan, vk, pics);

#ifdef __ANDROID__
    shaders->load("shaders", "list.xml", androidAssetManager);
#else
    shaders->load("shaders", "list.xml");
#endif

    shaders->shaders[1].use();

    glClearColor(0.5f, 0.5f, 0.6f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, -400, 400, orthoMatrix);


#ifndef __ANDROID__
    model.load("test.rm2");
    music->open("music/music.ogg");
  

#else
    model.load("test.rm2", androidAssetManager);
   
    music->open("music/music.ogg", androidAssetManager);
    LOGI("Inited\n");
#endif

    music->setVolume(sys->musicVolume);
    music->playback();


    srand((unsigned int)time(0));


    Animation idle;
    idle.startFrame = 0;
    idle.endFrame = 23;

    animations.add(idle);

    Animation punch;
    punch.startFrame = 59;
    punch.endFrame = 79;

    animations.add(punch);

    Animation death;
    death.startFrame = 102;
    death.endFrame = 116;

    animations.add(death);


    gameMode = TITLE;
    restartGame();

}

//-------------------
void Game::render()
{    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int MatrixID = shaders->shaders[1].getUniformID("ModelViewProjection");
    FlatMatrix identity;
    MatrixIdentity(identity.m);

    int MatrixID2 = shaders->shaders[0].getUniformID("ModelViewProjection");
    modelMatrixId = shaders->shaders[2].getUniformID("ModelViewProjection");


    FlatMatrix finalM = identity * orthoMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, finalM.m);
    glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, finalM.m);
   
    Render2D();
    pics->drawBatch(&shaders->shaders[1], &shaders->shaders[0], 666);
}

//---------------------
void Game::logic(){

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
    puts("Bye");
    GameProto::destroy();

    model.destroy();
}

//------------------------------------
void Game::GameLoop()
{
    switch(gameMode)
    {
    case TITLE : OnTitle(); break;
    case GAME  : OnGame(); break;
    }
}

//---------------------------

void Game::Render2D()
{
    switch(gameMode)
    {
    case TITLE : DrawTitle(); break;
    case GAME  : DrawGame(); break;
    }

    DrawDebugText();
}

//----------------------------
void Game::DrawDebugText(){

    char buf[256];
    sprintf(buf, "FPS:%d", fps());
    WriteText(2, 2, *pics, 0, buf, 0.8f, 0.8f);
}
