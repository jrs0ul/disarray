#ifndef _ENTITY_H
#define _ENTITY_H

#include "Vectors.h"


class Entity{

    Vector3D _pos;
    Vector3D _direction;
    Vector3D _up;
    Vector3D _right;

public:
    Entity();
    Vector3D pos(){return _pos;}
    Vector3D dir(){return _direction;}
    Vector3D up(){return _up;}
    Vector3D right(){return _right;}
    void setpos(Vector3D newpos);
    void setdir(Vector3D newdir){_direction = newdir;}
    void setup(Vector3D newup){_up = newup;}
    void setright(Vector3D newright){_right = newright;}

    void move(float d);
    void strife(float d);
    void yaw(float angle, bool use_up = true);
    void pitch(float angle, bool use_right = true);
    void roll(float angle, bool use_dir = true);
    void fly(float d, bool use_up = true);
    void makeMatrix(float* matrix);
    void makeBillBoard(Vector3D objectPos, float * matrix);


};



#endif //_ENTITY_H
