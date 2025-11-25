#include <disarray/ShaderLoader.h>
#include <disarray/VulkanVideo.h>
#include <disarray/SpriteBatcher.h>
#include <disarray/Xml.h>
#ifdef __ANDROID__
#include <android/log.h>
#include <android/log_macros.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

void ShaderLoader::init(bool useVulkan, VulkanVideo* vk, SpriteBatcher* pics)
{
    isVulkan = useVulkan;
    _vk = vk;
    _pics = pics;

}


#ifdef __ANDROID__
    bool ShaderLoader::load(const char* basedir, const char* listFile, AAssetManager* assman)
#else
    bool ShaderLoader::load(const char* basedir, const char* listFile)
#endif
{
    char buf[512];
    char shaderName[512];
    int useUvs = 0;
    int useAlphaBlend = 0;
    sprintf(buf, "%s/%s", basedir, listFile);
    Xml shaderList;

#ifdef __ANDROID__
    bool res = shaderList.load(buf, assman);
#else
    bool res = shaderList.load(buf);
#endif

    if (!res)
    {
        return res;
    }

    XmlNode *mainNode = shaderList.root.getNode(L"Shaders");

    int shaderCount = 0;

    if (mainNode)
    {
        shaderCount = mainNode->childrenCount();

        for (int i = 0; i < shaderCount; ++i)
        {
            XmlNode *node = mainNode->getNode(i);

            if (node)
            {

                if (wcscmp(node->getName(), L"Shader") != 0)
                {
                    continue;
                }

                if (node->attributeCount())
                {

                    for (unsigned j = 0; j < node->attributeCount(); ++j)
                    {
                        XmlAttribute* attr = node->getAttribute(j);

                        if (attr)
                        {
                            if (wcscmp(attr->getName(), L"name") == 0)
                            {
                                wchar_t* value = attr->getValue();

                                if (value)
                                {
                                    sprintf(shaderName, "%ls", value);
                                }
                            }
                            else if (wcscmp(attr->getName(), L"useUvs") == 0)
                            {
                                wchar_t* value = attr->getValue();

                                if (value)
                                {
                                    sprintf(buf, "%ls", value);
                                    useUvs = atoi(buf);
                                }
                            }
                            else if (wcscmp(attr->getName(), L"useAlphaBlend") == 0)
                            {
                                wchar_t* value = attr->getValue();

                                if (value)
                                {
                                    sprintf(buf, "%ls", value);
                                    useAlphaBlend = atoi(buf);
                                }
                            }


                        }

                    } //  attribute for



                    if (strlen(shaderName))
                    {
                        ShaderProgram shader;
#ifdef __ANDROID__
                        loadSingleShader(shaderName, shader, useUvs, useAlphaBlend, assman);
#else
                        loadSingleShader(shaderName, shader, useUvs, useAlphaBlend);
#endif
                        shaders.push_back(shader);
                    }

                } // has attributes

            }
        }
    }

    shaderList.destroy();

    return true;
}


#ifdef __ANDROID__
    void ShaderLoader::addShaderManualy(const char* name, bool useUvs, bool needAlphaBlend, AAssetManager* assman)
#else
    void ShaderLoader::addShaderManualy(const char* name, bool useUvs, bool needAlphaBlend)
#endif
{
    ShaderProgram newShader;
#ifdef __ANDROID__
    loadSingleShader(name, newShader, useUvs, needAlphaBlend, assman);
#else
    loadSingleShader(name, newShader, useUvs, needAlphaBlend);
#endif
    shaders.push_back(newShader);

}


void ShaderLoader::destroy()
{

    for (std::vector<ShaderProgram>::reverse_iterator it = shaders.rbegin(); it < shaders.rend(); ++it)
    {
        it->destroy((isVulkan) ? _vk->getDevice() : nullptr);
    }

    shaders.clear();

}

#ifdef __ANDROID__
void ShaderLoader::loadSingleShader(const char* name, ShaderProgram& shader, bool useUvs, bool needAlphaBlend, AAssetManager* assman)
#else
void ShaderLoader::loadSingleShader(const char* name, ShaderProgram& shader, bool useUvs, bool needAlphaBlend)
#endif
{

    shader.create(isVulkan);

    char error[1024];
    char buf[512];

    if (!isVulkan)
    {
        Shader vert;
        Shader frag;

        sprintf(buf, "shaders/%s.vert", name);
        printf("Loading vertex shader %s...\n", buf);

#ifdef __ANDROID__
        vert.loadGL(VERTEX_SHADER, buf, assman);
#else
        vert.loadGL(VERTEX_SHADER, buf);
#endif

        sprintf(buf, "shaders/%s.frag", name);
        printf("Loading fragment shader %s...\n", buf);
#ifdef __ANDROID__
        frag.loadGL(FRAGMENT_SHADER, buf, assman);
#else
        frag.loadGL(FRAGMENT_SHADER, buf);
#endif

        shader.attach(vert);
        shader.attach(frag);
        shader.link();

        shader.getLog(error, 1024);
        if (strlen(error))
        {
#ifdef __ANDROID__
            LOGI("--%s--", buf);
            LOGI("%s", error);
        }
        LOGI("---------------");
#else
            printf("--%s--\n", buf);
            puts(error);
        }
        puts("-----------");
#endif
    }
    else //VULKAN
    {
        VkDevice*         vulkanDevice = _vk->getDevice();
        VkPhysicalDevice* vkPhysicalDevice = _vk->getPhysicalDevice();
        VkRenderPass*     vkRenderPass = _vk->getRenderPass();

        Shader vert;
        Shader frag;

        sprintf(buf, "shaders/%s_vert.spv", name);
#ifdef __ANDROID__
        vert.loadVK(VERTEX_SHADER, buf, vulkanDevice, assman);
#else
        vert.loadVK(VERTEX_SHADER, buf, vulkanDevice);
#endif

        shader.attach(vert);

        sprintf(buf, "shaders/%s_frag.spv", name);
#ifdef __ANDROID__
        frag.loadVK(FRAGMENT_SHADER, buf, vulkanDevice, assman);
#else
        frag.loadVK(FRAGMENT_SHADER, buf, vulkanDevice);
#endif

        shader.attach(frag);
        shader.buildVkPipeline(vulkanDevice,
                                vkPhysicalDevice,
                                vkRenderPass,
                                (_pics) ? _pics->getVkDSL() : nullptr,
                                useUvs,
                                needAlphaBlend);
    }

}
