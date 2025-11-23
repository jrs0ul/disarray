/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2010
 -------------------------------------------
 Color data
 mod. 2010.09.27
 */
#ifndef _COLORS_H__
#define _COLORS_H__

#include <cstring>

struct COLOR{

    union
    {
        float c[4];

        struct
        {
            float r;
            float g;
            float b;
            float a;
        };
    };
    
    COLOR(float nr, float ng, float nb, float na = 1.0f){
        c[0] = nr; c[1] = ng; c[2] = nb; c[3] = na;
    }

    COLOR(const float* nc){
        if (nc)
            memcpy(c, nc, sizeof(float)*4);
    }

    COLOR(){
        c[0] = 1.0f; c[1] = 1.0f; c[2] = 1.0f; c[3] = 1.0f;
    }

};
#endif //_COLORS_H__
