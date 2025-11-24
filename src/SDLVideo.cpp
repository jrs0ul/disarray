/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2025
 -------------------------------------------
 SDL window for PC
 */

#include <disarray/SDLVideo.h>
#include <disarray/VulkanVideo.h>
#include <cstdio>

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <SDL_opengl.h>
    #else
        #include <SDL2/SDL_opengl.h>
    #endif
#else
    #ifdef __ANDROID__
            #include <GLES2/gl2.h>
            #include <GLES2/gl2ext.h>
    #else
            #include <SDL2/SDL_opengl.h>
    #endif
#endif



bool SDLVideo::initWindow(const char * title,
                          const char * iconPath,
                          bool isWindowed,
                          bool useVulkan)
{


    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        return false;
    }

    int rSize, gSize, bSize, aSize;
    rSize = gSize = bSize = aSize = 8;
    int buffsize = ((rSize + gSize + bSize + aSize + 7) & 0xf8);

    if (!useVulkan)
    {
        printf("Renderer: OPENGL\n");

        SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8);
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0 );
        SDL_GL_SetAttribute( SDL_GL_RED_SIZE, rSize );
        SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, gSize );
        SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, bSize );
        SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, aSize );
        SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, buffsize );

        _flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    }
    else  //Vulkan
    {
        printf("Renderer: VULKAN\n");
        SDL_Vulkan_LoadLibrary(nullptr);

        _flags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

    }

    if (!isWindowed){
        _flags = _flags |SDL_WINDOW_FULLSCREEN;
    }

    icon = SDL_LoadBMP(iconPath);

    mainWindow = SDL_CreateWindow(title, 
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         _width,
                         _height,
                         _flags );
    if( mainWindow == 0 )
    {
        puts("can't set video mode");
        return false;
    }

    if (icon)
    {
         SDL_SetWindowIcon(mainWindow, icon);
    }

    if (!useVulkan)
    {
        SDL_GL_CreateContext(mainWindow);
    }
    else //  Vulkan
    {
        uint32_t extensionCount;
        const char** extensionNames = 0;

        SDL_Vulkan_GetInstanceExtensions(mainWindow, &extensionCount, nullptr);
        extensionNames = new const char *[extensionCount];
        SDL_Vulkan_GetInstanceExtensions(mainWindow, &extensionCount, extensionNames);

        vk = new VulkanVideo();
        VkInstance* instance = vk->createInstance(extensionCount, extensionNames);

        if (!instance)
        {
            return false;
        }

        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(mainWindow, *instance, &surface);

        //int width, height = 0;
        //SDL_Vulkan_GetDrawableSize(mainWindow, &width, &height);

        vk->init(surface);
    }

    printf("Context created\n");

    return true;


}
//-------------------------------------------
void SDLVideo::setMetrics(unsigned w, unsigned h){
    _width = w;
    _height = h;
}

//--------------------------------------------
void SDLVideo::quit(bool useVulkan)
{
    if (useVulkan)
    {
        if (vk)
        {
            vk->destroy();
            delete vk;
            vk = nullptr;
        }
    }

    SDL_DestroyWindow(mainWindow);

    if (useVulkan)
    {
        SDL_Vulkan_UnloadLibrary();
    }

    SDL_Quit();
}

void SDLVideo::swap(bool useVulkan)
{
    if (!useVulkan)
    {
        glFlush();
        SDL_GL_SwapWindow(mainWindow);
    }
    else
    {
        vk->endRenderPass();
        vk->endCommandBuffer();
        vk->queueSubmit();
        vk->queuePresent();
    }
}

