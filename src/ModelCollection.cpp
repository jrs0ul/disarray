
#include "ModelCollection.h"


bool ModelCollection::load(const char* path, bool useVBO){
    FILE* file=0;
    file = fopen(path, "rb");
    if (file){

        int result = 0;

        result = fread(&_frameCount, 4, 1, file);

        int subsetcount = 0;

        result = fread(&subsetcount, 4, 1, file);

        //for all subsets in file
        for (int i = 0; i < subsetcount; i++){
            FlatMatrix mat;
            result = fread(mat.m, 4, 16, file);

            float stuff[9];
            result = fread(stuff,4,9,file);


            Matrix mx, my, mz, tr, sc;
            MatrixRotationAxis(-stuff[0], Vector3D(1,0,0),mx);
            MatrixRotationAxis(stuff[2], Vector3D(0,1,0),my);
            MatrixRotationAxis(stuff[1], Vector3D(0,0,1),mz);
            MatrixTranslation(stuff[3], stuff[5], -stuff[4], tr);
            MatrixScale(-stuff[6], stuff[8], -stuff[7], sc);
            Matrix mxy, mxyz, mxyzt, final;
            MatrixMultiply(mx, my, mxy);
            MatrixMultiply(mxy,mz, mxyz);
            MatrixMultiply(mxyz, tr, mxyzt);
            MatrixMultiply(mxyzt, sc, final);
            FlatMatrix m2 = Matrix2Flat(final);



            matrices.add(m2);


            AnimationSet newset;

            //for all frames in subset
            for (unsigned int i = 0; i < _frameCount; i++){
                RatModel nuliovas;
                //nuliovas.loadSubset(file);

                nuliovas.loadVerticesandNormals(file);
                if (i==0){

                    nuliovas.loadIndicesandUVs(file);
                }
                else{

                    nuliovas.copyIndices(newset.frames[0].indices, newset.frames[0]._facecount*3);
                    if (newset.frames[0]._isUvs)
                        nuliovas.copyUVs(newset.frames[0].uvs, newset.frames[0]._facecount*3*2);
                }


                if (useVBO)
                    nuliovas.createUnindexedMesh();
                else
                    nuliovas.createDisplayList();


                newset.frames.add(nuliovas);
            }
            subsets.add(newset);
        }

        fclose(file);

        return true;
    }


    return false;



}

//-----------------------------------------
bool ModelCollection::drawsubset(unsigned index, unsigned frame, bool useVBO){

    if (!subsets.count())
        return false;

    if (frame > subsets[index].frames.count() - 1)
        frame = 0;

    if (index > subsets.count())
        index = 0;


    if (useVBO){

        subsets[index].frames[frame].draw(true);

    }
    else{

        subsets[index].frames[frame].display();

    }

    return true;

}
//-------------------------------------------------------------------
bool    ModelCollection::drawSubsetOutline(unsigned int index, unsigned int frame, float width){

    if (!subsets.count())
        return false;


    glDisable (GL_LIGHTING);
    glDisable (GL_TEXTURE_2D);


    glPolygonMode (GL_BACK, GL_LINE);
    glLineWidth (width);

    glCullFace (GL_FRONT);
    glDepthFunc (GL_LEQUAL);

    glColor3f (0.0f, 0.0f, 0.0f);

    if (frame > subsets[index].frames.count() - 1)
        frame = 0;
    subsets[index].frames[frame].draw(true); //display();

    glDepthFunc (GL_LESS);
    glCullFace (GL_BACK);

    glPolygonMode (GL_FRONT, GL_FILL);


    glColor3f (1.0f, 1.0f, 1.0f);

    glEnable (GL_TEXTURE_2D);
    glEnable (GL_LIGHTING);
    return true;
}
//-------------------------------------------
RatModel* ModelCollection::getSubset(unsigned int index, unsigned int frame){


    if (index < subsets.count())
        return &subsets[index].frames[frame];
    else
        return 0;
}
//----------------------------------------
void ModelCollection::getMatrix(unsigned int index, FlatMatrix& mat){


    if (index < subsets.count())
        for (int i = 0;i<16;i++)
            mat.m[i] = matrices[index].m[i];

}

//----------------------------------
    void ModelCollection::destroy(){
        for (unsigned i = 0; i < subsets.count(); i++){
            for (unsigned a = 0; a < _frameCount; a++){
                subsets[i].frames[a].destroy();
            }
            subsets[i].frames.destroy();
        }
        subsets.destroy();
        matrices.destroy();
        _frameCount = 0;
    }

