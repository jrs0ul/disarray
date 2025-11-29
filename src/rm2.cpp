#include <disarray/rm2.h>

#ifdef WIN32
    #ifdef _MSC_VER
        #include <SDL.h>
        #include <SDL_opengl.h>
    #else
        #include <SDL/SDL.h>

        #include <SDL/SDL_opengl.h>
    #endif
#elif __ANDROID__
    #include <GLES2/gl2.h>
#else
    #include <SDL2/SDL.h>

    #include <SDL2/SDL_opengl.h>
#endif


#include <cstdio>

#ifndef __ANDROID__
    #include <disarray/Extensions.h>
#endif

//-----------------------------------------------
#ifndef __ANDROID__
void Model::load(FILE* f){
#else
void Model::load(AAsset* f){
#endif

#ifndef __ANDROID__
    if (!fread(&_vertexcount, sizeof(int), 1, f))
#else
    if (!AAsset_read(f, &_vertexcount, sizeof(int)))
#endif
    {
        return;
    }

    if (_vertexcount){

        //printf("%lu vertices\n", _vertexcount);
        verts = (float*)malloc((_vertexcount * 3 + 1) * sizeof(float));

#ifndef __ANDROID__
        if (!fread(verts, 4, _vertexcount*3, f))
#else
        if (!AAsset_read(f, verts, _vertexcount*3 * 4))
#endif
        {
            return;
        }

        normals = (float*)malloc((_vertexcount * 3 + 1) * sizeof(float));
#ifndef __ANDROID__
        if (!fread(normals, 4, _vertexcount * 3, f))
#else
        if (!AAsset_read(f, normals, _vertexcount * 3 * 4))
#endif
        {
            return;
        }
    }

#ifndef __ANDROID__
    if (!fread(&_facecount, 4, 1, f))
#else
    if (!AAsset_read(f, &_facecount, 4))
#endif
    {
        return;
    }
       //printf("%lu faces\n", _facecount);
    if (_facecount)
    {
        indices = (unsigned *)malloc((_facecount * 3+1)*sizeof(int));

#ifndef __ANDROID__
        if (!fread(indices, sizeof(int), _facecount*3, f))
#else
        if (!AAsset_read(f, indices, sizeof(int) * _facecount * 3))
#endif
        {
            return;
        }
        
        int hasUVS = 0;
#ifndef __ANDROID__
        if (!fread(&hasUVS, sizeof(int), 1, f))
#else
        if (!AAsset_read(f, &hasUVS, sizeof(int)))
#endif
        {
            hasUVS = 0;
        }

        isUVS = (hasUVS == 1)? true: false;
        //printf("hasUVS %d\n", hasUVS);

        if (isUVS)
        {
            // puts("reading uvs");
            uvs = (float*)malloc(_facecount*6*sizeof(float));

#ifndef __ANDROID__
            if (!fread(uvs, sizeof(float), _facecount*6, f))
#else
            if (!AAsset_read(f, uvs, sizeof(float) * _facecount * 6))
#endif
            {
                return;
            }
        }
    }
}
//----------------------------------------------
void Model::createUnindexedMesh(bool createVBO, bool useDynamic){

    long size = _facecount * 9 + _facecount * 9;

    if (_facecount > 0)
    {
        data = (float*)malloc(size * sizeof(float));
    }
    for (unsigned int i = 0; i < _facecount * 3; i++){

        data[i * 6]     = verts[indices[i]*3];
        data[i * 6 + 1] = verts[indices[i]*3 + 1];
        data[i * 6 + 2] = verts[indices[i]*3 + 2];
            
        data[i * 6 + 3] = normals[indices[i]*3];
        data[i * 6 + 4] = normals[indices[i]*3 + 1];
        data[i * 6 + 5] = normals[indices[i]*3 + 2];

    }
    if (!createVBO)
    {
        printf("exiting\n");
        return;
    }

#ifndef __ANDROID__
    if (!glGenBuffers){
        LoadExtensions();
    }
#endif

    /*if (glGenBuffers){
        if (VBOids[0] == 0){
            glGenBuffers(2, VBOids);
        }
        //printf("VBO id%u\n", VBOids[2]);
        GLenum de;
        de = (useDynamic) ? GL_DYNAMIC_DRAW_ARB : GL_STATIC_DRAW_ARB;

        glBindBuffer(GL_ARRAY_BUFFER_ARB, VBOids[0]);
        glBufferData(GL_ARRAY_BUFFER_ARB, size * sizeof(float), data, de);

        if (isUVS){
            glBindBuffer(GL_ARRAY_BUFFER_ARB, VBOids[1]);
            glBufferData(GL_ARRAY_BUFFER_ARB, _facecount*3*2*sizeof(float),
                            uvs, GL_STATIC_DRAW_ARB);
        }

        glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
    }*/
}
//----------------------------------------------
void Model::destroy(){

    if (data){
        free(data); data = 0;
    }

    if (uvs){
        free(uvs); uvs = 0;
    }
    if (normals){
        free(normals); normals = 0;
    }

    if (indices){
        free(indices); indices = 0;
    }

    if (verts){
        free(verts);
        verts = 0;
    }

    if (VBOids[0] != 0){
        glDeleteBuffers(2, VBOids);
        VBOids[0] = 0;
        VBOids[1] = 0;
    }
}
//----------------------------------------
#ifndef __ANDROID__
    bool AssignedBones::load(FILE *f){
#else
    bool AssignedBones::load(AAsset *f){
#endif

        int inflcount = 0;
#ifndef __ANDROID__
        if (!fread(&inflcount, sizeof(int), 1, f))
#else
        if (!AAsset_read(f, &inflcount, sizeof(int)))
#endif
        {
            return false;
        }

        //printf("assigned bones: %d\n", inflcount);
        float * weights = 0;
        int * indexes = 0;
        if (inflcount){
            weights = (float*)malloc((inflcount + 2) * sizeof(float));
            indexes = (int*)malloc((inflcount + 2) * sizeof(int));

#ifndef __ANDROID__
            if (!fread(indexes, sizeof(int), inflcount, f))
#else
            if (!AAsset_read(f, indexes, sizeof(int) * inflcount))
#endif
            {
                return false;
            }

#ifndef __ANDROID__
            if (!fread(weights, sizeof(float), inflcount, f))
#else
            if (!AAsset_read(f, weights, sizeof(float) * inflcount))
#endif
            {
                return false;
            }

            for (int i = 0; i < inflcount; i++){
                AssignedBoneToVertice abv;

                abv.boneID = indexes[i];
                abv.weight = weights[i];

                bones.add(abv);
            }
            free(indexes); indexes = 0;
            free(weights); weights = 0;
        }

        return true;

    }

//----------------------------------------
    void Subset::calcVertnNormal(unsigned frame, unsigned indice, Vector3D & v, Vector3D & n ){

        Vector3D mainnorm = Vector3D(0,0,0);
        Vector3D mainvert = Vector3D(0,0,0);


        for (unsigned long a = 0; a < vertsbones[indice].bones.count(); a++){ //for each bone

            Vector3D vert = Vector3D(mesh.verts[indice * 3],
                                     mesh.verts[indice * 3 + 1],
                                     mesh.verts[indice * 3 + 2]);

            Vector3D norm = Vector3D(mesh.normals[indice * 3],
                                     mesh.normals[indice * 3 + 1],
                                     mesh.normals[indice * 3 + 2]);

            float w = vertsbones[indice].bones[a].weight;
            unsigned index = vertsbones[indice].bones[a].boneID;

            vert.transform(anim[index].frames[frame].mat);

            //apply weight
            vert = Vector3D(vert.x * w, vert.y * w, vert.z * w);
            mainvert = mainvert + vert;
                //--------------

            Matrix nm;
            MatrixCopy(anim[index].frames[frame].mat, nm);
            nm[0][3] = 0; nm[1][3] = 0; nm[2][3] = 0; 
            nm[3][0] = 0; nm[3][1] = 0; nm[3][2] = 0;
            nm[3][3] = 1;
            norm.transform(nm);
            norm = Vector3D(norm.x * w, norm.y * w, norm.z * w);
            mainnorm = mainnorm + norm;
        }
            mainnorm.normalize();

            v = mainvert;
            n = mainnorm;

    }


//-----------------------------------------

    void Subset::drawVA(unsigned long frame, ShaderProgram* shader){
        
        if (frame >= _framecount)
            frame = _framecount - 1;

        if (_bonecount){
            for (unsigned int i = 0; i < mesh._facecount * 3; i++){
                Vector3D tmp2 = Vector3D(0,0,0);
                Vector3D tmp = Vector3D(0,0,0);

                calcVertnNormal(frame, mesh.indices[i], tmp, tmp2);

                mesh.data[i * 6]     = tmp.x;
                mesh.data[i * 6 + 1] = tmp.y;
                mesh.data[i * 6 + 2] = tmp.z;
               // printf("%f %f %f\n", tmp.x, tmp.y, tmp.z);

                mesh.data[i * 6 + 3] = tmp2.x;
                mesh.data[i * 6 + 4] = tmp2.y;
                mesh.data[i * 6 + 5] = tmp2.z;

            }
        }

        //printf("drawva\n");

        int attribID = shader->getAttributeID("aPosition"); 
        long vertexsize = 3 * sizeof(float) + 3 * sizeof(float);
        glVertexAttribPointer(attribID, 3, GL_FLOAT, GL_FALSE, vertexsize, mesh.data);
        glEnableVertexAttribArray(attribID);

        
        //glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_NORMAL_ARRAY);

        //if (mesh.isUVS)
        //    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        //float * offset = mesh.data;
        //long vertexsize = 3 * sizeof(float) + 3 * sizeof(float);

        //glVertexPointer(3, GL_FLOAT, vertexsize, offset);
        //offset += 3;
        //glNormalPointer(GL_FLOAT, vertexsize, offset);

        int UvsAttribID = 0;
        
        if (mesh.isUVS)
        {
            UvsAttribID = shader->getAttributeID("aUvs");
            glVertexAttribPointer(UvsAttribID, 2, GL_FLOAT, GL_FALSE, 0, mesh.uvs);
            glEnableVertexAttribArray(UvsAttribID);
        }
        //    glTexCoordPointer(2, GL_FLOAT, 0, mesh.uvs);

        glDrawArrays(GL_TRIANGLES, 0, mesh._facecount*3 );

        glDisableVertexAttribArray(attribID);
        //glDisableClientState(GL_VERTEX_ARRAY);
        //glDisableClientState(GL_NORMAL_ARRAY);
        if (mesh.isUVS)
        {
            glDisableVertexAttribArray(UvsAttribID);
        }
        //    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    }
//-------------------------------------------------------------
    void Subset::drawVBO(unsigned long frame){
        
     /*   if (frame >= _framecount)
            frame = _framecount - 1;

        if (_bonecount){

            glBindBuffer(GL_ARRAY_BUFFER_ARB, mesh.VBOids[0]);
            float* ptr = (float*)glMapBuffer(GL_ARRAY_BUFFER_ARB,
                                                GL_WRITE_ONLY_ARB);

            for (unsigned int i = 0; i < mesh._facecount * 3; i++){
                Vector3D norm = Vector3D(0,0,0);
                Vector3D vert = Vector3D(0,0,0);

                TransformedVertex * tf = &tvertices[mesh.indices[i]];
                if (tf->frame != (int)frame){
                    calcVertnNormal(frame, mesh.indices[i], vert, norm);
                    tf->frame = (int)frame;
                    memcpy(tf->vert, vert.v, 3 * sizeof(float));
                    memcpy(tf->norm, norm.v, 3 * sizeof(float));
                } 
                else{
                    memcpy(vert.v, tf->vert, 3 * sizeof(float));
                    memcpy(norm.v, tf->norm, 3 * sizeof(float));
                }
                memcpy(&ptr[i * 6], vert.v, 3 * sizeof(float));
                memcpy(&ptr[i * 6 + 3], norm.v, 3 * sizeof(float));
            }
            glUnmapBuffer(GL_ARRAY_BUFFER_ARB);
            glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        if (mesh.isUVS)
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        long vertexsize = (3 * sizeof(float)) * 2;
        glBindBuffer(GL_ARRAY_BUFFER_ARB, mesh.VBOids[0]);
        glVertexPointer(3, GL_FLOAT, vertexsize, (char*)NULL);
        glNormalPointer(GL_FLOAT, vertexsize, (char*)NULL + 12);

        if (mesh.isUVS){
            glBindBuffer(GL_ARRAY_BUFFER_ARB, mesh.VBOids[1]);
            glTexCoordPointer(2, GL_FLOAT, 0, (char*)NULL);
        }

        glDrawArrays(GL_TRIANGLES, 0, mesh._facecount*3 );
        glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        if (mesh.isUVS)
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
    }

//------------------------------------------
    void Subset::draw(unsigned long frame){

#ifndef __ANDROID__
        if (frame >= _framecount)
            frame = _framecount - 1;
        
        glBegin(GL_TRIANGLES);
        //printf("%lu\n", mesh._facecount);
        for (unsigned int i =0; i<mesh._facecount*3 ;i++){

            Vector3D tmp2 = Vector3D(0,0,0);
            Vector3D tmp = Vector3D(0,0,0);


            if (_bonecount){

                calcVertnNormal(frame, mesh.indices[i], tmp, tmp2);
            }
            else{
                tmp2 = Vector3D(mesh.normals[mesh.indices[i] * 3],
                                mesh.normals[mesh.indices[i] * 3 + 1],
                                mesh.normals[mesh.indices[i] * 3 + 2]);
                tmp = Vector3D(mesh.verts[mesh.indices[i] * 3],
                               mesh.verts[mesh.indices[i] * 3 + 1],
                               mesh.verts[mesh.indices[i] * 3 + 2]);

            }

            glNormal3f(tmp2.x,
                        tmp2.y,
                        tmp2.z);

            if (mesh.isUVS)
                glTexCoord2f(mesh.uvs[i*2], mesh.uvs[i*2+1]);

            glVertex3f(tmp.x,
                       tmp.y,
                       tmp.z);
        }
        glEnd();
#endif

    }

//----------------------------------------------------
void Subset::destroy(){

    mesh.destroy();
    //assigned bones
    for (unsigned i = 0; i < vertsbones.count(); i++){
        vertsbones[i].destroy();
    }
    vertsbones.destroy();
    //animation
    for (unsigned i = 0; i < anim.count(); i++){
        anim[i].destroy();
    }
    anim.destroy();
    //transformed vertices
    tvertices.destroy();
}
//------------------------------------------------
#ifndef __ANDROID__
    bool Subset::load(FILE * f, bool createVBO){
#else
    bool Subset::load(AAsset * f, bool createVBO) {
#endif

        if (!f)
        {
            return false;
        }

        //puts("loading model");
        mesh.load(f);
        mesh.createUnindexedMesh(createVBO);
        //puts("loading bones data");
#ifndef __ANDROID__
        if (!fread(&_bonecount, sizeof(int), 1, f))
#else
        if (!AAsset_read(f, &_bonecount, sizeof(int)))
#endif
        {
            return false;
        }

       // printf("bonecount: %lu\n", _bonecount);

        if (_bonecount)
        {
            for (unsigned a = 0; a < mesh._vertexcount; a++)
            {

                AssignedBones b;
                if (!b.load(f))
                {
                    return false;
                }

                vertsbones.add(b);
                TransformedVertex tv;
                tv.frame = -1;
                tvertices.add(tv);
            }
        }
        //----------------------------------------------

        //puts("loading framecount");
        int framec = 0;
#ifndef __ANDROID__
        if (!fread(&framec, sizeof(int), 1, f))
#else
        if (!AAsset_read(f, &framec, sizeof(int)))
#endif
        {
            return false;
        }

        _framecount = (unsigned long)framec;
        //printf("frames: %d\n", framec);

        for (unsigned b = 0; b < _bonecount; b++)
        {
            PoseBoneFrames bf;

            //read each frame
            for (int a = 0; a < framec; a++)
            {
                PoseBone nbone;
                float stuff[30];

#ifndef __ANDROID__
                if (!fread(stuff, sizeof(float), 16, f))
#else
                if (!AAsset_read(f, stuff, sizeof(float) * 16))
#endif
                {
                    return false;
                }

                int index = 0;

                for (int y = 0; y < 4; y++)
                {
                    for (int x = 0; x < 4; x++)
                    {
                        nbone.mat[y][x] = stuff[index];
                        index ++;
                    }
                }

                bf.frames.add(nbone);
            }
            anim.add(bf);
        }

        //printf("loaded animated bones: %lu\n", anim.count());

        //puts("Loaded");
        return true;
    }


//---------------------------------------------
#ifndef __ANDROID__
    bool SubsetCollection::load(const char* filename, bool createVBO){
#else
    bool SubsetCollection::load(const char* filename, AAssetManager* assman, bool createVBO){
#endif



#ifndef __ANDROID__
        FILE * modelfile = 0;
        modelfile = fopen(filename, "rb");
#else
        AAsset* modelfile = 0;
        modelfile = AAssetManager_open(assman, filename, AASSET_MODE_UNKNOWN );
#endif


        if (!modelfile)
        {
            printf("no such file: %s\n", filename);
            return false;
        }

        char magic[4];

#ifndef __ANDROID__
        if (!fread(magic, 1, 3, modelfile))
        {
            fclose(modelfile);
            return false;
        }
#else
        if (!AAsset_read(modelfile, magic, 3))
        {
            AAsset_close(modelfile);
            return false;
        }
#endif
        magic[3] = '\0';
        puts(magic);
        if (strcmp(magic,"RM2") != 0){
            puts("not RM2");
            return false;
        }
        
        unsigned subsetCount = 0;

#ifndef __ANDROID__
        if (!fread(&subsetCount, sizeof(int), 1, modelfile))
        {
            fclose(modelfile);
            return false;
        }
#else
        if (!AAsset_read(modelfile, &subsetCount, sizeof(int)))
        {
            AAsset_close(modelfile);
            return false;
        }
#endif


        printf("subset count %d\n", subsetCount);
        for (unsigned i = 0; i < subsetCount; i++){

            Subset newSubset;
            newSubset.load(modelfile, createVBO); 
            subsets.add(newSubset);

        }

#ifndef __ANDROID__
        fclose(modelfile);
#else
        AAsset_close(modelfile);
#endif

        return true;

    }
//--------------------------------------------
bool SubsetCollection::draw(unsigned subset, unsigned frame, ShaderProgram* shader, bool drawVBO)
{
        if (!subsets.count())
            return false;

        if (subset >= subsets.count())
            subset = 0;

        if (drawVBO)
            subsets[subset].drawVBO(frame);
        else
            subsets[subset].drawVA(frame, shader);

        return true;
    }
//--------------------------------------------
    bool SubsetCollection::drawOutline(unsigned subset,
                                       unsigned frame,
                                       float width){
#ifndef __ANDROID__
        if (!subsets.count())
            return false;

        bool enableLightAfter = false;

        if (glIsEnabled(GL_LIGHTING)){
            glDisable (GL_LIGHTING);
            enableLightAfter = true;
        }
        glDisable(GL_TEXTURE_2D);


        glPolygonMode (GL_BACK, GL_LINE);
        glLineWidth (width);

        int func = GL_LESS;
        glGetIntegerv(GL_DEPTH_FUNC, &func);

        glCullFace (GL_FRONT);
        glDepthFunc (GL_LEQUAL);

        glColor3f (0.0f, 0.0f, 0.0f);

        if (subset >= subsets.count())
            subset = 0;

        subsets[subset].drawVBO(frame);

        glDepthFunc (func);
        glCullFace (GL_BACK);

        glPolygonMode (GL_BACK, GL_FILL);

        glColor3f (1.0f, 1.0f, 1.0f);

        glEnable (GL_TEXTURE_2D);
        if (enableLightAfter)
            glEnable (GL_LIGHTING);
        glLineWidth (1.0f);

        return true;
#endif
    }

//--------------------------------------------
void SubsetCollection::drawall(unsigned frame, ShaderProgram* shader){

    for (unsigned i = 0; i < subsets.count(); i++){
        subsets[i].drawVA(frame, shader);
    }
}
//---------------------------------------------
void SubsetCollection::destroy(){
    for (unsigned i = 0; i < subsets.count(); i++){
        subsets[i].destroy();
    }
    subsets.destroy();
}
//---------------------------------------------
    void SubsetCollection::dump(const char * path){
        FILE * f;

        f = fopen(path, "wt+");
        if (f){
        /*    
            for (unsigned i = 0; i < armatures.count(); i++){
                fprintf(f, "Object %u:\n", i);

                for (unsigned a = 0; a < armatures[i].frames.count(); a++){
                    fprintf(f, "  Frame %u:\n", a);

                    for (unsigned x = 0; x < armatures[i].frames[a].pbones.count(); x++){

                        fprintf(f, "    %2.6f %2.6f %2.6f %2.6f\n",
                                armatures[i].frames[a].pbones[x].mat[0][0],
                                armatures[i].frames[a].pbones[x].mat[0][1],
                                armatures[i].frames[a].pbones[x].mat[0][2],
                                armatures[i].frames[a].pbones[x].mat[0][3]
                                );
                        fprintf(f, "    %2.6f %2.6f %2.6f %2.6f\n",
                                armatures[i].frames[a].pbones[x].mat[1][0],
                                armatures[i].frames[a].pbones[x].mat[1][1],
                                armatures[i].frames[a].pbones[x].mat[1][2],
                                armatures[i].frames[a].pbones[x].mat[1][3]
                                );
                        fprintf(f, "    %2.6f %2.6f %2.6f %2.6f\n",
                                armatures[i].frames[a].pbones[x].mat[2][0],
                                armatures[i].frames[a].pbones[x].mat[2][1],
                                armatures[i].frames[a].pbones[x].mat[2][2],
                                armatures[i].frames[a].pbones[x].mat[2][3]
                                );
                        fprintf(f, "    %2.6f %2.6f %2.6f %2.6f\n",
                                armatures[i].frames[a].pbones[x].mat[3][0],
                                armatures[i].frames[a].pbones[x].mat[3][1],
                                armatures[i].frames[a].pbones[x].mat[3][2],
                                armatures[i].frames[a].pbones[x].mat[3][3]
                                );
                        fprintf(f, "\n");
                    }
                }
            }*/

            for (unsigned i = 0; i < subsets.count(); i++){
                fprintf(f, "subset %u [\n", i);

                fputs("----------------------------------------------------\n", f);
                for (unsigned a = 0; a < subsets[i].anim.count(); a++){
                    fprintf(f, "bone: %u { \n", a);

                    for (unsigned y = 0; y < subsets[i].anim[a].frames.count(); y++){

                        if (y == 29){

                        PoseBone * pb = 0;
                        pb = &subsets[i].anim[a].frames[y];



                        fprintf(f, "%u.\n", y);

                        fprintf(f, "%2.3f %2.3f %2.3f %2.3f \n", 
                                pb->mat[0][0], pb->mat[0][1], pb->mat[0][2], pb->mat[0][3]);
                        fprintf(f, "%2.3f %2.3f %2.3f %2.3f \n",
                                pb->mat[1][0], pb->mat[1][1], pb->mat[1][2], pb->mat[1][3]);
                        fprintf(f, "%2.3f %2.3f %2.3f %2.3f \n",
                                pb->mat[2][0], pb->mat[2][1], pb->mat[2][2], pb->mat[2][3]);
                        fprintf(f, "%2.3f %2.3f %2.3f %2.3f \n",
                                pb->mat[3][0], pb->mat[3][1], pb->mat[3][2], pb->mat[3][3]);
                        fputs("\n",f);
                        }
                    }


                    fputs("}\n",f);

                }

                fputs("----------------------------------------------------\n", f);
                for (unsigned a = 0; a < subsets[i].vertsbones.count(); a++){
                    fprintf(f, "vertex(%u){ ", a);

                    float totalweight = 0.0f;

                    for ( unsigned z = 0; z < subsets[i].vertsbones[a].bones.count(); z++){

                        fprintf(f, "bone:%u w:%2.3f   ", 
                                subsets[i].vertsbones[a].bones[z].boneID,
                                subsets[i].vertsbones[a].bones[z].weight
                                );
                        totalweight += subsets[i].vertsbones[a].bones[z].weight;

                    }
                    fprintf(f, " =(%2.3f) }\n", totalweight);
                }
                fputs("]\n", f);
            }

            fclose(f);
        }
    }
//-----------------------------------
    unsigned long SubsetCollection::framecount(unsigned long subset){
        if (subset >= subsets.count())
            return 0;
        else
            return subsets[subset].framecount();
    }
//-----------------------------------
    unsigned long SubsetCollection::facecount(unsigned long subset){

        if (subset >= subsets.count())
            return 0;
        else
            return subsets[subset].mesh._vertexcount;
    }
