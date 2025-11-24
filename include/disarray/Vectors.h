/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2010
 -------------------------------------------
 Vectors
 mod. 2010.09.27
 */
#ifndef VECTORS_H
#define VECTORS_H


struct Vector3D{

    union
    {
        float v[4];
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
    };

    Vector3D(float nx, float ny, float nz){
        v[0] = nx; v[1] = ny; v[2] = nz; v[3] = 1.0f;
    }
    Vector3D(float nx, float ny, float nz, float nw){
        v[0] = nx; v[1] = ny; v[2] = nz; v[3] = nw;
    }
    Vector3D(){v[0] = 0; v[1] = 0; v[2] = 0; v[3] = 1.0f;}
    void set(float nx, float ny, float nz, float nw = 1.0f){
        v[0] = nx; v[1] = ny; v[2] = nz; v[3] = nw;
    }
    void transform(float matrix[4][4]);
    void transform(float *matrix);
    void normalize();
    float length();
    
    friend const Vector3D operator + (const Vector3D& left, const Vector3D& right );

    friend const Vector3D operator - (const Vector3D& left, const Vector3D& right );

    friend const Vector3D operator ^ (const Vector3D& left, //cross
                                const Vector3D& right);

    friend  float operator * (const Vector3D& left, //dot
                                const Vector3D& right);

};


#endif //VECTORS_H


