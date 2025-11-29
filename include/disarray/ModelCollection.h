
#ifndef _MODELCOLLECTION_H
#define _MODELCOLLECTION_H

#include "RatModel.h"
#include "../Matrix.h"



class AnimationSet{
public:
    DArray<RatModel> frames;

};

class ModelCollection{


        DArray<AnimationSet> subsets;
        DArray<FlatMatrix> matrices;

        unsigned _frameCount;


    public:
        void         destroy();
        bool         load(const char* path, bool useVBO = true);
        bool         drawsubset(unsigned int index = 0, unsigned int frame = 0, bool useVBO = true);
        bool         drawSubsetOutline(unsigned int index = 0, unsigned int frame = 0, float width = 2.5f);
        RatModel*    getSubset(unsigned index, unsigned frame = 0);
        void         getMatrix(unsigned int index, FlatMatrix& mat);
        unsigned int count(){return subsets.count();}
        void         transform(int index){glMultMatrixf(matrices[index].m);}
        int          frameCount(){return _frameCount;}

};


#endif //_MODELCOLLECTION_H
