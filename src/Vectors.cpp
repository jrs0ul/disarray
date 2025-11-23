/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2010
 -------------------------------------------
 Vectors
 mod. 2010.09.27
 */
#include <cmath>
#include <cstring>
#include "Vectors.h"

void Vector3D::transform(float matrix[4][4]){
    float nw[4];
    nw[0] = v[0] * matrix[0][0] + 
             v[1] * matrix[1][0] + 
             v[2] * matrix[2][0] +
             v[3] * matrix[3][0];
    nw[1] = v[0] * matrix[0][1] +
             v[1] * matrix[1][1] +
             v[2] * matrix[2][1] +
             v[3] * matrix[3][1];
    nw[2] = v[0] * matrix[0][2] +
             v[1] * matrix[1][2] +
             v[2] * matrix[2][2]+
             v[3] * matrix[3][2];

    nw[3] = v[0] * matrix[0][3] +
             v[1] * matrix[1][3] +
             v[2] * matrix[2][3]+
             v[3] * matrix[3][3];


     memcpy(v, nw, 4 * sizeof(float));
}
//---------------------------
void Vector3D::transform(float * matrix){
    float nw[4];
    nw[0] = v[0] * matrix[0] + 
            v[1] * matrix[4] + 
            v[2] * matrix[8] +
            v[3] * matrix[12];
    nw[1] = v[0] * matrix[1] +
            v[1] * matrix[5] +
            v[2] * matrix[9] +
            v[3] * matrix[13];
    nw[2] = v[0] * matrix[2] +
            v[1] * matrix[6] +
            v[2] * matrix[10]+
            v[3] * matrix[14];
    
    nw[3] = v[0] * matrix[3] +
            v[1] * matrix[7] +
            v[2] * matrix[11]+
            v[3] * matrix[15];
    
    
    memcpy(v, nw, 4 * sizeof(float));
}
//----------------------------
float Vector3D::length()
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
//----------------------------
void Vector3D::normalize(){
    float l = length();
    if (l == 0){return;}
    v[0] = v[0] / l;
    v[1] = v[1] / l;
    v[2] = v[2] / l;
}
//----------------------
const Vector3D operator ^ (const Vector3D& left,
                                const Vector3D& right){ //cross
        return Vector3D(left.v[1] * right.v[2] - left.v[2] * right.v[1],
                        left.v[2] * right.v[0] - left.v[0] * right.v[2],
                        left.v[0] * right.v[1] - left.v[1] * right.v[0] );
}
//----------------------
float operator * (const Vector3D& left,
                                const Vector3D& right){ //dot
        return left.v[0] * right.v[0] + 
               left.v[1] * right.v[1] + 
               left.v[2] * right.v[2];
}

//----------------------------------
const Vector3D operator - (const Vector3D& left, const Vector3D& right ){
    return Vector3D(left.v[0] - right.v[0],
                    left.v[1] - right.v[1],
                    left.v[2] - right.v[2]);
}
//--------------------------------------
const Vector3D operator + (const Vector3D& left, const Vector3D& right ){
    return Vector3D(left.v[0] + right.v[0],
                    left.v[1] + right.v[1],
                    left.v[2] + right.v[2]);
}


