#include "Gopnik.h"


void Gopnik::animate(DArray<Animation>& anims)
{
    animFrame++;

    if (animFrame > anims[currentAnimation].endFrame)
    {
        animFrame = anims[nextAnimation].startFrame;
        currentAnimation = nextAnimation;
        nextAnimation = 0;
    }


}

void Gopnik::draw(bool flip, 
                  int textureId,
                  SubsetCollection& model,
                  ShaderProgram& shader,
                  int   matrixUniform,
                  float* orthoMatrix)
{
    const float modelScale = 40.f;

    Matrix sc;
    MatrixScale(modelScale * (flip ? 1 : -1), modelScale, modelScale, sc);
    FlatMatrix scale = Matrix2Flat(sc);
    scale.m[12] = x;
    scale.m[13] = y;
    Matrix rt;
    MatrixRotationAxis(3.14f, Vector3D(0,0,1), rt);
    FlatMatrix rot = Matrix2Flat(rt);
    FlatMatrix finalM2 = rot * scale * orthoMatrix;
    glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, finalM2.m);

    
    glBindTexture(GL_TEXTURE_2D, textureId);
    model.drawall(animFrame, &shader);

}


void Gopnik::drawHudBars(SpriteBatcher& pics)
{
   //hp bar bg
    pics.draw(-1, x, y + 100 , 666, true, 102, 7, 0,
              COLOR(0,0,0, 0.8f), COLOR(0, 0, 0, 0.8f));

    int hp = (health < 0) ? 0 : health;

    float hpProportion = hp / (maxhealth * 1.f);

    float barSize = 100 * hpProportion;

    //hp bar
    pics.draw(-1,
              x - (100 * 0.5 - barSize * 0.5f),
              y + 100,
              666, true, 
              barSize, 5,
              0,
              COLOR(1,0,0,1), COLOR(1, 0, 0, 1));
 

}


