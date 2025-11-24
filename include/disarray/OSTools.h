/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2025
 -------------------------------------------
 some useful stuff
 */
#ifndef _UTILS_H
#define _UTILS_H


#ifdef _WIN32
    #include <windows.h>
#else
#ifndef __ANDROID__

#else
    #include <android/asset_manager.h>
#endif
    #include <iconv.h>
    #include <dirent.h>
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#include "DArray.h"


struct ptext{
    char t[255];
};

struct Lines{
    DArray<ptext> l;
};



//converts wide character string to ascii string with special characters
//which represent lithuanian letters. Usefull for bitmap font
void convertLT(const wchar_t* txt, char * buf, int len);

//multiplatform version of wcstok, in Windblows it uses wcstok with 3 
//arguments and with 2 in other oses
wchar_t * _wcstok(wchar_t * str, const wchar_t * delimiters, wchar_t** pointer);

//multiplatform function converts multi byte utf8 string (char*) to 
//wide char string (wchar_t *)
void      UTF8toWchar(char* utftext, wchar_t * wchartext);

//reads file to array, data is allocated in function
//use free() to free the memory
#ifdef __ANDROID__
long      ReadFileData(const char* path, char ** data, AAssetManager* man);
#else
long      ReadFileData(const char* path, char ** data);
#endif

//Multiplatform function gets user home path.
void    GetHomePath(char * _homePath);

//Multiplatform directory creation
void    MakeDir(const char * path);

void    GetFileList(const char * path, Lines& l);
void    GetDirectoryList(const char* path, Lines& l);



#endif //UTILS_H
