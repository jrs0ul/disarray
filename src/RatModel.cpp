
#include <stdio.h>
#include "RatModel.h"


#include <cstring>
#include "../Vectors.h"



void RatModel::destroy(){

    if (adj){
        puts("valom valom");

        for (unsigned int i=0;i<_adjcount;i++){

            adj[i].faces.destroy();

        }
        delete []adj;
        adj=0;
        puts("isvalem");
    }

    if (verts){
        delete []verts;
        verts=0;
    }
    if (normals){
        delete []normals;
        normals=0;
    }


    if (indices){
        delete []indices;
        indices=0;
    }


    if (uvs){
        delete []uvs;
        uvs=0;
    }

    //experimental

    if (unormals){
        delete []unormals;
        unormals = 0;
    }
    if (uverts){
        delete []uverts;
        uverts = 0;
    }

    if (glDeleteBuffersARB)
        glDeleteBuffersARB(3, VBOids);

}
//------------------------------------

void RatModel::loadVerticesandNormals(FILE* file){
    if (file){

        int result = 0;
        result = fread(&_vertexcount,4,1,file);
        if (_vertexcount > 0){

            if (verts){
                delete []verts;
                verts = 0;
            }

            verts = new float[_vertexcount*3];
            result = fread(verts,sizeof(float),_vertexcount*3,file);

            if (normals){
                delete []normals;
                normals = 0;
            }

            normals = new float[_vertexcount*3];
            result = fread(normals,sizeof(float),_vertexcount*3,file);
        }

    }
}
//------------------------------------
void RatModel::loadIndicesandUVs(FILE* file){

    if (file){

        int result = 0;

        result = fread(&_facecount, 4, 1, file);

        indices = new unsigned int [_facecount * 3];


        result = fread(indices, sizeof(int), _facecount*3, file);


        //printf("faces :%d\n",_facecount);
        //puts("reading uvs");
        //-----------------------
        int isUvs_=0;
        result = fread(&isUvs_,4,1,file);

        if (isUvs_){
          //  puts("uvs found");
            _isUvs=true;
            uvs = new float[_facecount*3*2];
            result = fread(uvs,sizeof(float),_facecount*3*2,file);
        }

    }


}
//------------------------------------
void RatModel::loadSubset(FILE* file){

    if (file){

        int result = 0;
        result = fread(&_vertexcount,4,1,file);
        verts = new float[_vertexcount*3];
        result = fread(verts,sizeof(float),_vertexcount*3,file);

        normals = new float[_vertexcount*3];
        result = fread(normals,sizeof(float),_vertexcount*3,file);

        //-------

        result = fread(&_facecount,4,1,file);

        indices = new unsigned int [_facecount * 3];

        result = fread(indices,sizeof(int),_facecount*3,file);


        //printf("faces :%d\n",_facecount);
        //puts("reading uvs");

        int isUvs_=0;
        result = fread(&isUvs_,4,1,file);

        if (isUvs_){
          //  puts("uvs found");
            _isUvs=true;
            uvs = new float[_facecount*3*2];
            result = fread(uvs,sizeof(float),_facecount*3*2,file);
        }

        //----
        createDisplayList();

    }

}



//---------------------------------------

void RatModel::display(bool USE_LIST /*= true*/){
    if (USE_LIST)
        glCallList(dList);
    else
        draw();
}

//-----------------------------------------------------
void RatModel::drawNormals(){

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);

        for (unsigned int i = 0; i<_facecount*3 ;i++){
            glColor3f(1.0,1.0,1.0);
            glBegin(GL_LINES);
            glVertex3f(verts[indices[i]*3],verts[indices[i]*3+1],verts[indices[i]*3+2]);
                glVertex3f(verts[indices[i]*3]-normals[indices[i]*3],
                            verts[indices[i]*3+1]-normals[indices[i]*3+1],
                                verts[indices[i]*3+2]-normals[indices[i]*3+2]);
                glEnd();



        }
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);



}

//-------------------------------------------------------------
void RatModel::createUnindexedMesh(){


    //puts("Trying to create UnIndexed mesh");
    if (_facecount * 3 * 3){
        uverts   = new float[_facecount * 3 * 3];
        unormals = new float[_facecount * 3 * 3];
    }
    else
        return;

    for (unsigned int i =0; i < _facecount*3 ;i++){

        unormals[i * 3]     = normals[indices[i]*3];
        unormals[i * 3 + 1] = normals[indices[i]*3 + 1];
        unormals[i * 3 + 2] = normals[indices[i]*3 + 2];

        uverts[i * 3]     = verts[indices[i]*3];
        uverts[i * 3 + 1] = verts[indices[i]*3 + 1];
        uverts[i * 3 + 2] = verts[indices[i]*3 + 2];

    }


    if (!glGenBuffersARB){
        //puts("reloading extensions");
        LoadExtensions();
       // puts("ok");
    }
    if (glGenBuffersARB){

        if (VBOids[0] == 0){
            //puts("making VBO");

            glGenBuffersARB(3, VBOids);
        }
        //printf("VBO id%u\n", VBOids[2]);


        glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOids[0]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, _facecount*3*3*sizeof(float), uverts, GL_STATIC_DRAW_ARB);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOids[1]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, _facecount*3*3*sizeof(float), unormals, GL_STATIC_DRAW_ARB);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOids[2]);
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, _facecount*3*2*sizeof(float), uvs, GL_STATIC_DRAW_ARB);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

       // puts("ok");
    }

   // puts("Unindexed mesh created");

}
//-------------------------------------------------------

void RatModel::VAdraw(){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);


    if (_isUvs)
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);



    glVertexPointer(3, GL_FLOAT, 0, uverts);
    glNormalPointer(GL_FLOAT, 0, unormals);

    if (_isUvs)
        glTexCoordPointer(2, GL_FLOAT, 0, uvs);


    glDrawArrays(GL_TRIANGLES, 0, _facecount*3 );

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

}

//----------------------------------
void RatModel::VBOdraw(){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    if (_isUvs)
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);




    glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOids[0]);
    glVertexPointer(3, GL_FLOAT, 0, (char *)NULL);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOids[1]);
    glNormalPointer(GL_FLOAT, 0, (char *)NULL);

    if (_isUvs){
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, VBOids[2]);
        glTexCoordPointer(2, GL_FLOAT, 0, (char *)NULL);
    }

    glDrawArrays(GL_TRIANGLES, 0, _facecount*3);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    if (_isUvs)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}



//------------------------------------------------------------
void RatModel::draw(bool USE_ARRAYS /*= true*/){

    if (USE_ARRAYS){
        if (glBindBufferARB)
            VBOdraw();
        else
            VAdraw();
    }
    else{ //------------------glbegin/glend

        glBegin(GL_TRIANGLES);



        for (unsigned int i =0; i<_facecount*3 ;i++){

            glNormal3f(normals[indices[i]*3],normals[indices[i]*3+1],normals[indices[i]*3+2]);
            if (_isUvs){

                glTexCoord2f(uvs[i*2],uvs[i*2+1]); //taip gerai
            }
                //glTexCoord2f(uvs[indices[i]*2],uvs[indices[i]*2+1]); //o taip negerai
            glVertex3f(verts[indices[i]*3], verts[indices[i]*3 + 1], verts[indices[i]*3 + 2]);

        }
        glEnd();
    }


}
//------------------------------------------------------
void RatModel::copyVerts(const float * _verts, long count){

    if (verts)
        delete []verts;
    verts = new float[count];
    memcpy(verts, _verts, count*sizeof(float));

    _vertexcount = count;



}

//--------------------------------------------------
void RatModel::copyIndices(const unsigned int* _indices, long count){
    if (indices)
        delete []indices;
    indices = new unsigned int[count];
    memcpy(indices, _indices, count*sizeof(unsigned int));
    _facecount = count/3;


}
//------------------------------------------------------------
void RatModel::copyUVs(const float *UVs, long count){
    if (uvs)
        delete uvs;
    uvs = new float [count];
    memcpy( uvs, UVs, count*sizeof(float));
    _isUvs = true;
}

//-----------------------------------------------------------
void RatModel::copyNormals(const float* _norms, long count){

    if (normals)
        delete []normals;
    normals = new float[count];
    memcpy(normals, _norms, count*sizeof(float));

}
//---------------------------------------------------------
void RatModel::copyAdjacency(Adj* _adj, long count){

     if (adj){
        for (unsigned int i=0; i < _adjcount; i++)
            adj[i].faces.destroy();
        delete []adj;
        adj=0;
    }

    adj = new Adj[count];
    for (int i=0;i<count;i++)
        for (unsigned int a=0;a<_adj[i].faces.count();a++)
            adj[i].faces.add(_adj[i].faces[a]);

    _adjcount = count;


}


//----------------------------------------------------------
void RatModel::dump(const char* file ){

    FILE * f;
    f = fopen(file, "wt+");
    if (f){
        for (unsigned i = 0; i < _vertexcount; i+=3)
            fprintf(f, "%.3f %.3f %.3f\n", verts[i], verts[i + 1], verts[i + 2]);
            fputs("\n", f);
        fprintf(f,"indices(%lu):\n",  _facecount*3);
        for (unsigned i = 0; i < _facecount; i++){
            fprintf(f, "%d %d %d\n", indices[i*3], indices[i*3+1], indices[i*3+2]);
        }
        fputs("\n", f);
        fprintf( f, "normals(%lu):\n",_vertexcount*3);
        for (unsigned i = 0; i < _vertexcount; i += 3){
            fprintf(f, "%.3f %.3f %.3f\n", normals[i], normals[i + 1], normals[i + 2]);
        }

        if (adj){
            fprintf(f,"adjacency\n\n");
            int l = 0;
            for (unsigned i = 0; i < _vertexcount; i+=3){
                fprintf(f,"vertex %d neighbours: ",l);
                for (unsigned int a = 0; a<adj[l].faces.count();a++){
                  fprintf(f, "%d ",adj[l].faces[a]);
                }
                fprintf(f, "\n");
                l++;
            }

            fprintf(f,"\n");
        }
    }

    fclose(f);

}
//---------------------------------------
void RatModel::computeNormals(){

    Vector3D * faceNormals = 0;

    faceNormals = new Vector3D[_facecount];

    //printf("facecount: %u\n",_facecount);

    for (unsigned i = 0; i<_facecount; i++){
        Vector3D v1, v2, v3, a, b;

        v3 = Vector3D(verts[indices[i*3+2]*3], verts[indices[i*3+2]*3+1], verts[indices[i*3+2]*3+2]);
        v2 = Vector3D(verts[indices[i*3+1]*3], verts[indices[i*3+1]*3+1], verts[indices[i*3+1]*3+2]);
        v1 = Vector3D(verts[indices[i*3]*3], verts[indices[i*3]*3+1], verts[indices[i*3]*3+2]);

       // printf("%.3f %.3f %.3f\n",verts[indices[i*3]*3],verts[indices[i*3]*3+1],verts[indices[i*3]*3+2]);

        a = v3 - v2;
        b = v1 - v2;

        faceNormals[i] = a ^ b;

        faceNormals[i].normalize();
        //printf("%.3f %.3f %.3f\n",faceNormals[i].x(),faceNormals[i].y(),faceNormals[i].z());
    }



    unsigned index = 0;


    for (unsigned long a = 0; a < vertexCount(); a += 3){
        DArray<Vector3D> fac;

        unsigned normalIndex = 0;

        if (!adj){
            for (unsigned z = 0; z< _facecount; z++){
                if ((indices[z*3] == index)||
                    (indices[z*3+1] == index)||
                    (indices[z*3+2] == index)){

                    fac.add(faceNormals[z]);

                    if (indices[z*3] == index)
                        normalIndex = indices[z*3];
                    if (indices[z*3+1] == index)
                        normalIndex = indices[z*3+1];
                    if (indices[z*3+2] == index)
                        normalIndex = indices[z*3+2];

                }
            }
       }
        else{
            for (unsigned b = 0; b < adj[index].faces.count(); b++){
                fac.add(faceNormals[adj[index].faces[b]]);
            }
             normalIndex = index;
        }




        Vector3D sum(0.0f, 0.0f, 0.0f);
        for (unsigned a = 0; a < fac.count(); a++){
            sum = sum + fac[a];
        }

        sum = Vector3D(sum.x()/fac.count(),sum.y()/fac.count(),sum.z()/fac.count());

        sum.normalize();

        normals[normalIndex*3] = sum.x();
        normals[normalIndex*3+1] = sum.y();
        normals[normalIndex*3+2] = sum.z();

        fac.destroy();

        ++index;

    }


    delete []faceNormals;


}


//---------------------------------------
void RatModel::createDisplayList(bool useArray){

    dList = glGenLists (1);
    glNewList(dList, GL_COMPILE);

    draw(useArray);

    glEndList();

}
