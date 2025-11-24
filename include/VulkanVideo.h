#pragma once

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <SDL.h>
        #include <SDL_vulkan.h>
        #include <vulkan/vulkan.hpp>
    #else
        #include <SDL2/SDL.h>
    #endif
#elif __ANDROID__
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #include <vulkan/vulkan.h>
    #include <vector>
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_vulkan.h>
  #include <vulkan/vulkan.hpp>
#endif


class VulkanVideo
{


public:
    static const bool USE_VALIDATION_LAYER = false;

    VkInstance*       createInstance(uint32_t extensionCount, const char** extensionNames);
    bool              init(VkSurfaceKHR& surface, bool useDepth = false);
    void              getNextSwapImage();
    void              beginCommandBuffer();
    void              resetCommandBuffer();
    void              endCommandBuffer();
    void              beginRenderPass(VkClearColorValue clearColor, VkClearDepthStencilValue clearDepthStencil);
    void              endRenderPass();
    void              setViewportAndScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    void              queueSubmit();
    void              queuePresent();

    void              destroy();

    VkDevice*         getDevice()             {return &vkDevice;}
    VkCommandBuffer*  getCommandBuffer()      {return &vkCommandBuffer;}
    VkRenderPass*     getRenderPass()         {return &vkRenderPass;}
    VkPhysicalDevice* getPhysicalDevice()     {return &vkPhysicalDevice;}
    VkCommandPool*    getCommandPool()        {return &vkCommandPool;}
    VkQueue*          getGraphicsQueue()      {return &vkGraphicsQueue;}
    uint32_t          getSwapChainImageCount(){return vkSwapchainImageCount;}




    static uint32_t findMemoryType(VkPhysicalDevice& physical, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    static void createImage(VkDevice& device,
                            VkPhysicalDevice& physical,
                            uint32_t width, uint32_t height,
                            VkFormat format, VkImageTiling tiling, 
                            VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, 
                            VkDeviceMemory& imageMemory);

    static void createBuffer(VkDevice& device,
                            VkPhysicalDevice& physical,
                            VkDeviceSize size,
                            VkBufferUsageFlags usage,
                            VkMemoryPropertyFlags properties,
                            VkBuffer& buffer,
                            VkDeviceMemory& bufferMemory);

    static VkImageView createImageView(VkDevice& device,
                                       VkImage& image,
                                       VkFormat format,
                                       VkImageAspectFlags aspectFlags);

private:

    bool               buildFrameBuffers();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkBool32           getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);
    void createSemaphore(VkSemaphore *semaphore);




    std::vector<VkFence>         vkFences;
    std::vector<VkCommandBuffer> vkCommandBuffers;
    std::vector<VkImage>         vkSwapchainImages;
    std::vector<VkImageView>     vkSwapchainImageViews;
    std::vector<VkFramebuffer>   vkSwapchainFramebuffers;
    VkImageView                  vkDepthImageView;
    VkDevice                     vkDevice;
    VkPhysicalDevice             vkPhysicalDevice;
    VkInstance                   vkInstance;

    VkImage                      depthImage;
    VkDeviceMemory               depthImageMemory;


    VkSwapchainKHR               vkSwapchain;
    VkCommandBuffer              vkCommandBuffer;
    VkSurfaceFormatKHR           vkSurfaceFormat;
    VkSurfaceCapabilitiesKHR     vkSurfaceCapabilities;
    VkFormat                     vkDepthFormat;
    VkRenderPass                 vkRenderPass;
    VkCommandPool                vkCommandPool;
    VkSemaphore                  vkImageAvailableSemaphore;
    VkSemaphore                  vkRenderingFinishedSemaphore;
    VkQueue                      vkGraphicsQueue;
    VkQueue                      vkPresentQueue;
    uint32_t                     vkFrameIndex;
    uint32_t                     vkSwapchainImageCount;

    uint32_t                     surfaceWidth;
    uint32_t                     surfaceHeight;
    uint32_t                     imageWidth;
    uint32_t                     imageHeight;
    bool                         _useDepth;

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};


    static const char *toStringMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT s) {
        switch (s) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                return "VERBOSE";
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                return "ERROR";
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                return "WARNING";
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                return "INFO";
            default:
                return "UNKNOWN";
        }
    }
    static const char *toStringMessageType(VkDebugUtilsMessageTypeFlagsEXT s) {
        if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
            return "General | Validation | Performance";
        if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
            return "Validation | Performance";
        if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
            return "General | Performance";
        if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
            return "Performance";
        if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT))
            return "General | Validation";
        if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) return "Validation";
        if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) return "General";
        return "Unknown";
    }


    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void * /* pUserData */) {
        auto ms = toStringMessageSeverity(messageSeverity);
        auto mt = toStringMessageType(messageType);
        printf("[%s: %s]\n%s\n", ms, mt, pCallbackData->pMessage);

        return VK_FALSE;
    }



    static void populateDebugMessengerCreateInfo(
            VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }



};
