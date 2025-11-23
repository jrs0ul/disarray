/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2025
 -------------------------------------------
 SDL window
 */

#ifndef _SDL_VIDEO_WRAP_H
#define _SDL_VIDEO_WRAP_H

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <SDL.h>
        #include <SDL_vulkan.h>
        #include <vulkan/vulkan.hpp>
    #else
        #include <SDL2/SDL.h>
    #endif
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_vulkan.h>
  #include <vulkan/vulkan.hpp>
#endif

class VulkanVideo;

class SDLVideo
{

    int           _bpp;
    int           _flags;

    SDL_Surface*  icon;
    SDL_Window*   mainWindow;

    VulkanVideo*  vk;

    unsigned      _width;
    unsigned      _height;

public:

    SDLVideo(){_width = 640; _height = 480; vk = nullptr;}

    unsigned     width(){return _width;}
    unsigned     height(){return _height;}
    int          colorBits(){return _bpp;}
    int          flags(){return _flags;}
    void         setMetrics(unsigned w, unsigned h);

    bool         initWindow(const char * title,
                            const char * iconPath,
                            bool isWindowed = true,
                            bool useVulkan = true);
    void         quit(bool useVulkan);

    void         beginRenderPass(bool useVulkan);
    void         swap(bool useVulkan);

    VulkanVideo* getVkVideo(){return vk;}
};


#endif //_SDL_VIDEO_WRAP_H

