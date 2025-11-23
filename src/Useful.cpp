#include <cmath>
#include "Useful.h"
#include "Matrix.h"


//-----------------------------------------
bool CirclesColide(float x1,float y1,float radius1, float x2, float y2, float radius2){

    float difx=(float)fabs(x1-x2);
    float dify=(float)fabs(y1-y2);

    float distance=(float)sqrt(difx*difx+dify*dify);
    if (distance<(radius1+radius2))
        return true;


    return false;
}
//-------------------------------------------------------------------
Vector3D MakeVector(float speedx, float speedy, float _angle )
{
    Matrix rot;
    Vector3D pn = Vector3D(speedx,0, speedy);
    MatrixRotationY(_angle, rot);
    pn.transform(rot);
    return Vector3D(pn.x, pn.z, 0.f);


}

//--------------------------------------------------------------------

Vector3D* Line(int x1, int y1, int x2, int y2, int& count, int gridw){

    bool steep = abs(y2 - y1) > abs(x2 - x1);
    if (steep){
        int tmp=x1;
        x1=y1; y1=tmp;
        tmp=x2;
        x2=y2; y2=tmp;
    }
    if (x1 > x2){
        int tmp=x1;
        x1=x2; x2=tmp;
        tmp=y1;
        y1=y2; y2=tmp;

    }
    int deltax = x2 - x1;
    int deltay = abs(y2 - y1);
    float error = 0.0f;
    float deltaerr = deltay / (deltax*1.0f);
    int ystep;
    int y = y1;
    if (y1 < y2)
        ystep = 1*gridw;
    else 
        ystep = -1*gridw;

    count=0;
    Vector3D* mas=0;

    for (int x=x1;x<x2;x+=gridw){

        Vector3D* tmp=0;
        if (mas){
            tmp = new Vector3D[count];

            for (int i=0;i<count;i++)
            {
                tmp[i]=mas[i];
            }

            delete []mas;
        }
        (count)++;
        mas = new Vector3D[count];

        if (count>1)
        {
            for (int i=0;i<count-1;i++)
                mas[i]=tmp[i];
            delete []tmp;
        }

        if (steep)
        {
            mas[count-1].x=y/gridw;
            mas[count-1].y=x/gridw;
        }
        else
        {
            mas[count-1].x=x/gridw;
            mas[count-1].y=y/gridw;
        }

        error = error + deltaerr;

        if (error >= 0.5f)
        {
            y = y + ystep;
            error = error - 1.0f;
        }
    }
    return mas;
}
