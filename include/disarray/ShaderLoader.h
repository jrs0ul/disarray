#pragma once

#include <vector>
#include <disarray/ShaderProgram.h>

class ShaderLoader
{
public:
    std::vector<ShaderProgram> shaders;

#ifdef __ANDROID__
    bool load(const char* basedir, const char* listFile, AAssetManager& assman);
#else
    bool load(const char* basedir, const char* listFile);
#endif

    void destroy();


private:
    void loadSingleShader(const char* name, ShaderProgram& shader);

};
