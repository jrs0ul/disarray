#include <disarray/ShaderLoader.h>
#include <disarray/Xml.h>

#ifdef __ANDROID__
    bool ShaderLoader::load(const char* basedir, const char* listFile, AAssetManager& assman)
#else
    bool ShaderLoader::load(const char* basedir, const char* listFile)
#endif
{
    char buf[512];
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
                    XmlAttribute* attr = node->getAttribute(0);

                    if (attr)
                    {
                        if (wcscmp(attr->getName(), L"name") == 0)
                        {

                            wchar_t* value = attr->getValue();
                            if (value)
                            {
                                sprintf(buf, "%ls", value);
                                ShaderProgram shader;
                                loadSingleShader(buf, shader);
                                shaders.push_back(shader);
                            }
                        }
                    }
                }

            }
        }
    }

    shaderList.destroy();

    return true;
}

void ShaderLoader::destroy()
{

    for (std::vector<ShaderProgram>::reverse_iterator it = shaders.rbegin(); it < shaders.rend(); ++it)
    {
        it->destroy();
    }

    shaders.clear();

}

void ShaderLoader::loadSingleShader(const char* name, ShaderProgram& shader)
{

    shader.create(false);
    char error[1024];
    char buf[512];
    Shader vert;
    Shader frag;
    printf("Loading vertex shader...\n");
    sprintf(buf, "shaders/%s.vert", name);
#ifdef __ANDROID__
    vert.loadGL(VERTEX_SHADER, buf, AssetManager);
#else
    vert.loadGL(VERTEX_SHADER, buf);
#endif

    printf("Loading fragment shader...\n");
    sprintf(buf, "shaders/%s.frag", name);
#ifdef __ANDROID__
    frag.loadGL(FRAGMENT_SHADER, buf, AssetManager);
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
