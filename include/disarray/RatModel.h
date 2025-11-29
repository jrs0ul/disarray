
#ifndef RATMODEL_H
#define RATMODEL_H


#ifdef WIN32
    #ifdef _MSC_VER
        #include <SDL_opengl.h>
    #else
        #include <SDL/SDL_opengl.h>
    #endif
#else
    #include <SDL/SDL_opengl.h>
#endif

#include "../DArray.h"
#include <cstdio>
#include "../Extensions.h"


class ModelCollection;

struct Adj{
    DArray<int> faces;
};

//-------------------------------------
class RatModel{

    friend class ModelCollection;

    float*         verts; //virsunes * 3
    float*         normals; //normales * 3
    unsigned int* indices;
    float*         uvs;     //teksturu koordinates * 2
    unsigned long  _vertexcount;
    unsigned long  _facecount;
    unsigned long  _adjcount; //virsuniu skaicius

    //unindexed stuff *experimental*
    float*         uverts; //faces * 3 * 3
    float*         unormals; //faces * 3 * 3
    //-------

    unsigned      VBOids[3];


    bool           _isUvs; //ar turi teksturu koordinates ?
    unsigned int   dList; //display list'as
    Adj*           adj; //bendri poligonai virsunei
    unsigned long _indexcount;



    void           loadSubset(FILE* file); //deprecated ?

    void           loadVerticesandNormals(FILE* file);
    void           loadIndicesandUVs(FILE* file);


public:

    RatModel(){verts=0; normals=0; /*faces=0;*/ _vertexcount=0; _facecount=0;
                uvs=0; adj = 0; indices=0; _isUvs=false;
                _indexcount = 0;
                //-----------------
                uverts = 0;
                unormals = 0;
                VBOids[0] = 0;
                VBOids[1] = 0;
                VBOids[2] = 0;

              }
    void            destroy();
    unsigned long vertexCount(){return _vertexcount;}
    unsigned long faceCount(){return _facecount;}

    bool            isUvs(){return _isUvs;} //ar turi teksturu koordinates
    void            draw(bool USE_ARRAYS = true);    // vertex arays OR glbegin/glend
    void            drawNormals();
    void            display(bool USE_LIST = true); //paišyti display lista arba ne
    unsigned int    indice(unsigned long index){return indices[index];}
    float           vertexcoord(unsigned long index){return verts[index];}

    void            copyVerts(const float* _verts, long count);
    float*          getVerts(){return verts;}
    void            copyAdjacency( Adj* _adj, long count);
    void            copyIndices(const unsigned int* _indices, long count);
    void            copyNormals(const float* _norms, long count);
    void            copyUVs(const float* UVs, long count);

    void            computeNormals();

    void            dump(const char* file);

    void            createDisplayList(bool useArray=false);


    void            createUnindexedMesh();
    void            VAdraw();
    void            VBOdraw();



};




#endif //RATMODEL_H

