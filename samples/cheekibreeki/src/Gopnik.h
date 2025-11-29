#ifndef GOPNIK_H
#define GOPNIK_H

#include <disarray/DArray.h>
#include <disarray/rm2.h>
#include <disarray/SpriteBatcher.h>

struct Animation
{
    int  startFrame;
    int  endFrame;
};


struct Gopnik
{
    float x;
    float y;
    int   health;
    int   maxhealth;
    int   animFrame;
    int   currentAnimation;
    int   nextAnimation;

    void animate(DArray<Animation>& anims);
    void draw(bool flip, 
              int textureId,
              SubsetCollection& model,
              ShaderProgram& shader,
              int   matrixUniform,
              float* orthoMatrix);
    void drawHudBars(SpriteBatcher& pics);
};


#endif //GOPNIK_H
