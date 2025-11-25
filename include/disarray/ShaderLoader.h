#pragma once

#include <vector>
#include <disarray/ShaderProgram.h>

class SpriteBatcher;
class VulkanVideo;

class ShaderLoader
{
    bool isVulkan;
    VulkanVideo* _vk;
    SpriteBatcher* _pics;
public:
    std::vector<ShaderProgram> shaders;

    void init(bool useVulkan, VulkanVideo* vk, SpriteBatcher* pics);

#ifdef __ANDROID__
    bool load(const char* basedir, const char* listFile, AAssetManager& assman);
#else
    bool load(const char* basedir, const char* listFile);
#endif


#ifdef __ANDROID__
    void addShaderManualy(const char* name, bool useUvs, bool needAlphaBlend, AAssetManager& assman);
#else
    void addShaderManualy(const char* name, bool useUvs, bool needAlphaBlend);
#endif

    void destroy();

private:
#ifdef __ANDROID__
    void loadSingleShader(const char* name, ShaderProgram& shader, bool useUvs, bool needAlphaBlend, AAssetManager& assman);
#else
    void loadSingleShader(const char* name, ShaderProgram& shader, bool useUvs, bool needAlphaBlend);
#endif

};
