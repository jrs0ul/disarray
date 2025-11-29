

#ifndef _RATMODELV2_H
#define _RATMODELV2_H

#include "DArray.h"
#include "Vectors.h"
#include "Matrix.h"
#include "ShaderProgram.h"



//--------------------

    struct PoseBone{
        Matrix mat;
    };
    //------------------
    
    struct PoseBoneFrames{

        DArray<PoseBone> frames;

        void destroy(){frames.destroy();}
    };
    //---------------------
    struct AssignedBoneToVertice{
        unsigned boneID;
        float weight;
    };

    //-------------------------
    struct AssignedBones{
        DArray<AssignedBoneToVertice> bones;

        void destroy(){bones.destroy();}

#ifndef __ANDROID__
        bool load(FILE *f);
#else
        bool load(AAsset *f);
#endif
    };

//----------------------------
    struct TransformedVertex{
        float vert[3];
        float norm[3];
        int frame;

        TransformedVertex(){
            vert[0] = 0; vert[1] = 0; vert[2] = 0;
            norm[0] = 0; norm[1] = 0; norm[2] = 0;
        }
    };

//=========================================
//Model data, vertices, normals etc.
class Model{
public:
    float * verts;
    float * normals;
    float * uvs;
    unsigned int * indices;
    unsigned long _vertexcount;
    unsigned long _facecount;

    float * data;
    //two vbos, one for geometry
    //another fo uvs
    unsigned      VBOids[2];

    bool isUVS;

    Model(){
        verts = 0;
        normals = 0;
        indices = 0;
        uvs = 0;
        _vertexcount = 0;
        _facecount = 0;
        isUVS = false;

        data = 0;
        VBOids[0] = 0;
        VBOids[1] = 0;
    }
#ifndef __ANDROID__
    void load(FILE* f);
#else
    void load(AAsset* f);
#endif
    //create mesh without indices. Each uvs represents a separate vertex
    void createUnindexedMesh(bool createVBO = true, bool useDynamic = true);
    void destroy();
};

//=========================================
//Single mesh in model file
class Subset{
    unsigned long _framecount;
    unsigned long _bonecount;
public:
    //anim[bone].frames[frame].mat <- pose matrix
    DArray<PoseBoneFrames> anim;
    Model mesh; //model data
    //bones assigned to each vertice, so
    //count of this DArray should be equal vertex count
    DArray<AssignedBones> vertsbones; 
    //already transformed vertices, fo speedup
    DArray<TransformedVertex> tvertices;

         Subset(){ _framecount = 0; _bonecount = 0;}
#ifndef __ANDROID__
    bool load(FILE * f, bool createVBO = false);
#else
    bool load(AAsset * f, bool createVBO = false);
#endif
    void draw(unsigned long frame);
    void drawVA(unsigned long frame, ShaderProgram* shader);
    void drawVBO(unsigned long frame);
    void calcVertnNormal(
                         unsigned frame,
                         unsigned indice,
                         Vector3D & v, 
                         Vector3D & n );
    unsigned long framecount(){return _framecount;}
    unsigned long bonecount(){return _bonecount;}
    void destroy(); 
};

//=========================================
//Collection of meshes
class SubsetCollection{
        DArray<Subset> subsets;
    public:
#ifndef __ANDROID__
        bool load(const char* filename, bool createVBO = false);
#else
        bool load(const char* filename, AAssetManager* assman, bool createVBO = false);
#endif
        bool draw(unsigned subset, unsigned frame, ShaderProgram* shader,
                  bool drawVBO = false);
        bool drawOutline(unsigned subset = 0,
                         unsigned frame = 0,
                         float width = 2.0f);
        void drawall(unsigned frame, ShaderProgram* shader);
        void destroy();
        void dump(const char * path);
        unsigned long count(){return subsets.count();}
        unsigned long framecount (unsigned long subset);
        unsigned long facecount(unsigned long subset);

};

#endif //RATMODELV2_H
