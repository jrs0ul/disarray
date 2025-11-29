
#include "Entity.h"
#include "Matrix.h"


Entity::Entity(){
    _pos.set(0.0f, 0.0f, 0.0f); _direction.set(0.0f, 0.0f, 1.0f); _up.set(0.0f, 1.0f, 0.0f);
    _right.set(1.0f, 0.0f, 0.0f);
}

//-----------------------------
void Entity::setpos(Vector3D newpos){
    _pos.set(newpos.x, newpos.y, newpos.z);
}

//--------------------------
void Entity::move(float d){
    Vector3D v(_direction.x * d, _direction.y * d, _direction.z * d);
    _pos = v + _pos;
}
//--------------------------
void Entity::strife(float d){
    Vector3D v(_right.x * d, _right.y * d, _right.z * d);
    _pos = _pos + v;
}


//--------------------------
void Entity::yaw(float angle, bool use_up){

    Matrix mat;
    if (use_up)
    {
        MatrixRotationAxis(angle,_up,mat);
    }
    else
    {
        MatrixRotationY(angle,mat);
    }

    _direction.transform(mat);
    if (!use_up){
        _up.transform(mat);
        _up.normalize();
    }
    _right.transform(mat);

    _right.normalize();
    _direction.normalize();


}
//--------------------------
void Entity::pitch(float angle, bool use_right){
    Matrix mat;
    if (use_right)
        MatrixRotationAxis(angle,_right,mat);
    else
        MatrixRotationAxis(angle,Vector3D(0,0,1),mat);

    _direction.transform(mat);
    _up.transform(mat);
    _direction.normalize();
    _up.normalize();
    if (!use_right){
        _right.transform(mat);
        _right.normalize();
    }
}
//--------------------------
void Entity::roll(float angle, bool use_dir){
     Matrix mat;
     if (use_dir)
        MatrixRotationAxis(angle,_direction,mat);
    else
        MatrixRotationAxis(angle,Vector3D(1,0,0),mat);

    _right.transform(mat);
    _up.transform(mat);
    _right.normalize();
    _up.normalize();
    if (!use_dir){
        _direction.transform(mat);
        _direction.normalize();
    }
}
//-----------------------------

void Entity::fly(float d, bool use_up){

    Vector3D v;
    if (use_up)
        v = Vector3D(_up.x * d, _up.y * d, _up.z * d);
    else
        v = Vector3D(0, d, 0);
    _pos = _pos + v;
}

//-------------------------------------
void Entity::makeMatrix(float* matrix){

   matrix[0] =  _right.x;     matrix[1] = _right.y;     matrix[2] = _right.z;      matrix[3] = 0;
   matrix[4] =  _up.x;        matrix[5] = _up.y;        matrix[6] = _up.z;         matrix[7] = 0;
   matrix[8] =  _direction.x; matrix[9] = _direction.y; matrix[10] = _direction.z; matrix[11] = 0;
   matrix[12] = _pos.x;       matrix[13] = _pos.y;      matrix[14] = _pos.z;       matrix[15] = 1;

}
//-------------------------------------
//Fills array with transformation matrix
//which transforms object to always face camera(entity)
void Entity::makeBillBoard(Vector3D objectPos, float * matrix){

    //negative to camera direction
    Vector3D myDir = _direction - _direction - _direction;
    myDir.normalize();
    //crossproduct between camera up and new direction
    Vector3D myRight = _up ^ myDir;
    myRight.normalize();
    //crossproduct between new direction and new right vector
    Vector3D myUp = myDir ^ myRight;
    myUp.normalize();


    matrix[0] =  myRight.x;     matrix[1] = myRight.y;     matrix[2] = myRight.z;      matrix[3] = 0;
    matrix[4] =  myUp.x;        matrix[5] = myUp.y;        matrix[6] = myUp.z;         matrix[7] = 0;
    matrix[8] =  myDir.x;       matrix[9] = myDir.y;       matrix[10] = myDir.z;       matrix[11] = 0;
    matrix[12] = objectPos.x;   matrix[13] = objectPos.y;  matrix[14] = objectPos.z;   matrix[15] = 1;

}


