/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2017
 -------------------------------------------
 some usefull stuff
 mod. 2010.09.27
 */
#include <cstring>
#include <cmath>
#include <cwchar>
#include <cstdlib>
#ifdef __ANDROID__
    #include <android/log.h>
    #include "AndroidFile.h"
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#endif
#include "OSTools.h"




#ifndef __ANDROID__
//----------------------------------------------------
void convertLT(const wchar_t* txt, char * buf, int len) {
    
        wchar_t LTletters[] = L"ĄąČčĘęĖėĮįŠšŲųŪūŽž";

        unsigned char Codes[] = {161, 177, 200, 232, 202, 234, 204, 236, 199, 231, 169,
                             185, 217, 249, 222, 254, 174, 190};

        if (txt){
            char * bufs = 0;
            int _len = 0;
            _len = wcslen(txt);
            if (_len){
                bufs = (char *)malloc(_len + 1);
    
                for (int i = 0; i <= _len; i++) {
                    bool found = false;
                    for (int a = 0; a < (int)wcslen(LTletters); a++){
                        if (txt[i] == LTletters[a]){
                            bufs[i] = Codes[a];
                            found = true;
                            break;
                        }
                    }
                    if (!found){
                        bufs[i] = (char)txt[i];
                    }
                }
                bufs[_len] = '\0';
                if (len < (int)strlen(bufs))
                    bufs[len] = '\0';
                strncpy(buf, bufs, len);
                //printf("|%s|len %d\n",buf, _len);
                free(bufs);
            }
        }
}
#endif
//-------------------------------------------------------------
wchar_t * _wcstok(wchar_t * str, const wchar_t * delimiters, wchar_t** pointer){
        wchar_t * result = 0;
        #ifdef WIN32
            result = wcstok(str, delimiters);
        #else
            result = wcstok(str, delimiters, pointer);
        #endif
        return result;
}
//----------------------------------------------------------
void      UTF8toWchar(char* utftext, wchar_t * wchartext){

        size_t utftextLen = 0;
        if (utftext){
            utftextLen = strlen(utftext);

            #ifdef WIN32
                int len = 0;
                len = MultiByteToWideChar(CP_UTF8, 0, utftext, utftextLen ,
                                    wchartext, utftextLen);
                wchartext[len] = L'\0';



            #elif __ANDROID__

            swprintf(wchartext, utftextLen, L"%hs", utftext);
            #else
            iconv_t cd;
                cd = iconv_open("WCHAR_T", "UTF-8");
                if (cd == (iconv_t)-1){
                    wchartext[0] = L'\0';
                    return;
                }

                if (cd){
                    if (utftextLen > 0){
                        size_t s2 = utftextLen * sizeof(wchar_t);
                        char * in = utftext;
                        char * out = (char *)wchartext;
                        size_t result = 0;
                        result = iconv(cd, &in, (size_t*)&utftextLen, &out, (size_t*)&s2);
                        if (result)
                        {
                        }
                        //if (s2 >= sizeof (wchar_t))
                        *((wchar_t *) out) = L'\0';

                    }
                    iconv_close(cd);
                }
            #endif
        }
}

//-------------------------------------------------------------
#ifdef __ANDROID__
    long  ReadFileData(const char* path, char ** data, AAssetManager* man)
#else
    long  ReadFileData(const char* path, char ** data)
#endif
    {
        if (*data)
        {
            return -1;
        }

        FILE * f = 0;

#ifndef __ANDROID__
        f = fopen(path, "rb");
#else
        LOGI("Opening file: %s", path);
        f = android_fopen(man, path, "rb");
#endif
        if (!f)
        {
            return -1;
        }

        long fsize = 0;

        fseek (f, 0, SEEK_END);
        fsize = ftell(f);
        rewind(f);

        if (fsize <= 0)
        {
            return -1;
        }

        (*data) = (char *)malloc(fsize + 10);


        if (!fread(*data, 1, fsize, f))
        {
            free(*data);
            (*data) = 0;

            fclose(f);
            return -1;
        }

        fclose(f);

        (*data)[fsize - 1] = '\0';

        return fsize;

    }
//-------------------------------------------------------------
void GetHomePath(char * _homePath){

        char * home = 0;
        char * homepath = 0;
        char * homedrive = 0;

        home = getenv("HOME");
        if (!home){
            homedrive = getenv("HOMESHARE");
            if (!homedrive)
                homedrive = getenv("HOMEDRIVE");
            homepath = getenv("HOMEPATH");

            if ((homepath)&&(homedrive)){
                sprintf(_homePath, "%s%s/", homedrive, homepath);
            }
            else
                sprintf(_homePath, "./");
        }
        else{
            sprintf(_homePath, "%s/", home);
        }
}
//---------------------------------------------------------------
void    GetFileList(const char* path, Lines& l){
#ifndef WIN32
    DIR* Dir;
    dirent *DirEntry = 0;
    Dir = opendir(path);
    DirEntry = readdir(Dir);
    while(DirEntry){
        ptext pt;
        strncpy(pt.t, DirEntry->d_name, 255);
        l.l.add(pt);
        
        DirEntry = readdir(Dir);
    }
    closedir(Dir);
#else
    HANDLE hFind;
    WIN32_FIND_DATA FindData;

    char buf[255];
    sprintf(buf,"%s/*", path);
    hFind = FindFirstFile(buf, &FindData);

    if(hFind != INVALID_HANDLE_VALUE){
            ptext pt;
            strncpy(pt.t, FindData.cFileName, 255);
            l.l.add(pt);
    }
    while (FindNextFile(hFind, &FindData)){

            ptext pt;
            strncpy(pt.t, FindData.cFileName, 255);
            l.l.add(pt);
    }

    FindClose(hFind);
#endif


}

//---------------------------------------------------------------
void    GetDirectoryList(const char* path, Lines& l){

#ifndef WIN32
    unsigned char isFolder = 0x4;
    DIR* Dir;
    dirent *DirEntry = 0;
    Dir = opendir(path);
    DirEntry = readdir(Dir);
    while(DirEntry){
        if ( DirEntry->d_type == isFolder){
            ptext pt;
            strncpy(pt.t, DirEntry->d_name, 255);
            l.l.add(pt);
        }
        
        DirEntry = readdir(Dir);
    }
    closedir(Dir);
#else
    HANDLE hFind;
    WIN32_FIND_DATA FindData;

    char buf[255];
    sprintf(buf,"%s/*", path);
    hFind = FindFirstFile(buf, &FindData);

    if(hFind != INVALID_HANDLE_VALUE){
        if (FindData.dwFileAttributes & 0x0010){
            ptext pt;
            strncpy(pt.t, FindData.cFileName, 255);
            l.l.add(pt);
        }
    }
    while (FindNextFile(hFind, &FindData)){

        if (FindData.dwFileAttributes & 0x0010){
            ptext pt;
            strncpy(pt.t, FindData.cFileName, 255);
            l.l.add(pt);

        }
    }

    FindClose(hFind);
#endif

    
}
//----------------------------------------------------------------
void MakeDir(const char * path){
    #ifdef WIN32
        CreateDirectory(path, 0);
    #else
        mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR);
    #endif

}

