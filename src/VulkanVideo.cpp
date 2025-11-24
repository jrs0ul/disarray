#include <disarray/VulkanVideo.h>
#include <disarray/MathTools.h>

#ifdef __ANDROID__
#include <android/log.h>
#include <android/log_macros.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

VkInstance* VulkanVideo::createInstance(uint32_t extensionCount, const char** extensionNames)
{
    bool layerFound = false;

    if (USE_VALIDATION_LAYER)
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


        for (const char *layerName : validationLayers) {
            layerFound = false;
            for (const auto &layerProperties: availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
        }
    }


    VkApplicationInfo appInfo{};
    appInfo.pEngineName = "Disarray";
    appInfo.apiVersion = VK_API_VERSION_1_0;


     VkInstanceCreateInfo instanceInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // sType
        nullptr,                                // pNext
        0,                                      // flags
        &appInfo,                               // pApplicationInfo
        0,                                      // enabledLayerCount
        nullptr,                                // ppEnabledLayerNames
        extensionCount,                         // enabledExtensionCount
        extensionNames,                         // ppEnabledExtensionNames
    };

     if (layerFound)
     {
         VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
         instanceInfo.enabledLayerCount =
             static_cast<uint32_t>(validationLayers.size());
         instanceInfo.ppEnabledLayerNames = validationLayers.data();
         populateDebugMessengerCreateInfo(debugCreateInfo);
         instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
     }

    VkResult res = vkCreateInstance(&instanceInfo, nullptr, &vkInstance);

    if (res != VK_SUCCESS)
    {
#ifdef __ANDROID__
    LOGE(
#else
    printf(
#endif
            "failed to create vulkan instance!");
        return nullptr;
    }

    return &vkInstance;
}

//================

bool VulkanVideo::init(VkSurfaceKHR& surface, bool useDepth)
{

    _useDepth = useDepth;
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices.data());

    bool physicalDeviceAssigned = false;

    for (auto device : physicalDevices)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
#ifdef __ANDROID__
            LOGI(
#else
            printf(
#endif
                    "We'll be using: %s!\n", properties.deviceName);
            vkPhysicalDevice = device;
            physicalDeviceAssigned = true;
            break;
        }
    }

    if (!physicalDeviceAssigned)
    {
        for (auto device : physicalDevices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);


            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            {
#ifdef __ANDROID__
                LOGI(
#else
                printf(
#endif
                "We'll be using: %s!\n", properties.deviceName);
                vkPhysicalDevice = device;
                physicalDeviceAssigned = true;
                break;
            }
        }
    }

    if (!vkPhysicalDevice)
    {
        return false;
    }

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t graphicsQueueIndex = UINT32_MAX;
    uint32_t presentQueueIndex = UINT32_MAX;

    VkBool32 support;
    uint32_t i = 0;

    for (VkQueueFamilyProperties queueFamily : queueFamilies) 
    {
        if (graphicsQueueIndex == UINT32_MAX && 
                queueFamily.queueCount > 0 && 
                queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueIndex = i;
        }
        if (presentQueueIndex == UINT32_MAX) 
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, surface, &support);

            if(support)
            {
                presentQueueIndex = i;
            }
        }
        ++i;
    }

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
        nullptr,                                    // pNext
        0,                                          // flags
        graphicsQueueIndex,                         // graphicsQueueIndex
        1,                                          // queueCount
        &queuePriority,                             // pQueuePriorities
    };

    VkPhysicalDeviceFeatures deviceFeatures = {};
    const char* deviceExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,   // sType
        nullptr,                                // pNext
        0,                                      // flags
        1,                                      // queueCreateInfoCount
        &queueInfo,                             // pQueueCreateInfos
        0,                                      // enabledLayerCount
        nullptr,                                // ppEnabledLayerNames
        1,                                      // enabledExtensionCount
        deviceExtensionNames,                   // ppEnabledExtensionNames
        &deviceFeatures,                        // pEnabledFeatures
    };

    vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice);

    vkGetDeviceQueue(vkDevice, graphicsQueueIndex, 0, &vkGraphicsQueue);

    vkGetDeviceQueue(vkDevice, presentQueueIndex, 0, &vkPresentQueue);


    //Let's create a swap chain
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, surface, &vkSurfaceCapabilities);

    surfaceWidth  = imageWidth = vkSurfaceCapabilities.currentExtent.width;
    surfaceHeight =  imageHeight = vkSurfaceCapabilities.currentExtent.height;


    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, surface, &formatCount, surfaceFormats.data());
    }

    std::vector<VkPresentModeKHR> presentModes;
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, surface, &presentModeCount, presentModes.data());
    }

    vkSurfaceFormat = chooseSwapSurfaceFormat(surfaceFormats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(presentModes);




    uint32_t imageCount = vkSurfaceCapabilities.minImageCount + 1;

    if (vkSurfaceCapabilities.maxImageCount > 0 && imageCount > vkSurfaceCapabilities.maxImageCount)
    {
        imageCount = vkSurfaceCapabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount    = vkSurfaceCapabilities.minImageCount;
    createInfo.imageFormat      = vkSurfaceFormat.format;
    createInfo.imageColorSpace  = vkSurfaceFormat.colorSpace;
    createInfo.imageExtent      = {imageWidth, imageHeight};
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;


    uint32_t queueFamilyIndices[] = {graphicsQueueIndex, presentQueueIndex};

    if (graphicsQueueIndex != presentQueueIndex)
    {
        createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices   = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    //Let's find what kind of composite alpha the device supports
    std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (auto& flag : compositeAlphaFlags)
    {
        if (vkSurfaceCapabilities.supportedCompositeAlpha & flag)
        {
            createInfo.compositeAlpha = flag;
            break;
        }
    }
    //----
    createInfo.presentMode    = presentMode;
    createInfo.clipped        = VK_TRUE;

    if (vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &vkSwapchain) != VK_SUCCESS)
    {
        return false;
    }

    vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &vkSwapchainImageCount, nullptr);
    vkSwapchainImages.resize(vkSwapchainImageCount);
    vkGetSwapchainImagesKHR(vkDevice, vkSwapchain, &vkSwapchainImageCount, vkSwapchainImages.data());

    vkSwapchainImageViews.resize(vkSwapchainImages.size());

    for (uint32_t i = 0; i < vkSwapchainImages.size(); i++)
    {
        vkSwapchainImageViews[i] = VulkanVideo::createImageView(vkDevice,
                                                                vkSwapchainImages[i],
                                                                vkSurfaceFormat.format,
                                                                VK_IMAGE_ASPECT_COLOR_BIT);
    }

    if (useDepth)
    {
        getSupportedDepthFormat(vkPhysicalDevice, &vkDepthFormat);

        VulkanVideo::createImage(vkDevice,
                                 vkPhysicalDevice,
                                 imageWidth,
                                 imageHeight,
                                 vkDepthFormat,
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                 depthImage,
                                 depthImageMemory);

        vkDepthImageView = VulkanVideo::createImageView(vkDevice,
                                                        depthImage,
                                                        vkDepthFormat,
                                                        VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    //render pass

    std::vector<VkAttachmentDescription> attachments;

    VkAttachmentDescription desc{};
    desc.flags = 0;
    desc.format = vkSurfaceFormat.format;
    desc.samples = VK_SAMPLE_COUNT_1_BIT;
    desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachments.push_back(desc);

    if (useDepth)
    {

        VkAttachmentDescription depthDesc{};
        depthDesc.flags = 0;
        depthDesc.format = vkDepthFormat;
        depthDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        depthDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachments.push_back(depthDesc);
    }

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;

    if (useDepth)
    {
        VkAttachmentReference depthReference = {};
        depthReference.attachment = 0;
        depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        subpassDescription.pDepthStencilAttachment = &depthReference;

    }

    std::vector<VkSubpassDependency> dependencies(1);

    dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass      = 0;
    dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    vkCreateRenderPass(vkDevice, &renderPassInfo, nullptr, &vkRenderPass);

    if (!buildFrameBuffers())
    {
        return false;
    }
    //command pool

    VkCommandPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolCreateInfo.queueFamilyIndex = graphicsQueueIndex;

    if (vkCreateCommandPool(vkDevice, &poolCreateInfo, nullptr, &vkCommandPool) != VK_SUCCESS)
    {
        return false;
    }

    //command buffers
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = vkCommandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = vkSwapchainImageCount;

    vkCommandBuffers.resize(vkSwapchainImageCount);
    if (vkAllocateCommandBuffers(vkDevice, &allocateInfo, vkCommandBuffers.data()) != VK_SUCCESS)
    {
        return false;
    }
    //---
    createSemaphore(&vkImageAvailableSemaphore);
    createSemaphore(&vkRenderingFinishedSemaphore);

    vkFences.resize(vkSwapchainImageCount);

    for(uint32_t i = 0; i < vkSwapchainImageCount; i++)
    {
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &vkFences[i]);
    }

    return true;

}

//=================


bool VulkanVideo::buildFrameBuffers()
{

    vkSwapchainFramebuffers.resize(vkSwapchainImageViews.size());

    for (size_t i = 0; i < vkSwapchainImageViews.size(); i++)
    {

        std::vector<VkImageView> attachments = {vkSwapchainImageViews[i]};

        if (_useDepth)
        {
            attachments.push_back(vkDepthImageView);
        }

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vkRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = imageWidth;
        framebufferInfo.height = imageHeight;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(vkDevice, &framebufferInfo, nullptr, &vkSwapchainFramebuffers[i]) != VK_SUCCESS)
        {
            return false;
        }
    }

    return true;

}


void VulkanVideo::getNextSwapImage()
{



    vkAcquireNextImageKHR(vkDevice,
                          vkSwapchain,
                          UINT64_MAX,
                          vkImageAvailableSemaphore,
                          VK_NULL_HANDLE,
                          &vkFrameIndex);

    vkWaitForFences(vkDevice, 1, &vkFences[vkFrameIndex], VK_FALSE, UINT64_MAX);
    vkResetFences(vkDevice, 1, &vkFences[vkFrameIndex]);


    vkCommandBuffer = vkCommandBuffers[vkFrameIndex];

}


void VulkanVideo::beginCommandBuffer()
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);
}


void VulkanVideo::resetCommandBuffer()
{
    vkResetCommandBuffer(vkCommandBuffer, 0);
}


void VulkanVideo::endCommandBuffer()
{
    vkEndCommandBuffer(vkCommandBuffer);
}

//=================

void VulkanVideo::beginRenderPass(VkClearColorValue clearColor, VkClearDepthStencilValue clearDepthStencil)
{
    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass        = vkRenderPass;
    render_pass_info.framebuffer       = vkSwapchainFramebuffers[vkFrameIndex];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = {imageWidth, imageHeight};

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDepthStencil;

    render_pass_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    render_pass_info.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(vkCommandBuffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

}


void VulkanVideo::setViewportAndScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    VkViewport viewport{};
    viewport.x = (float)x;
    viewport.y = (float)y;
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(vkCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor = { {(int)x,(int)y},
                         {width, height}};

    vkCmdSetScissor(vkCommandBuffer, 0, 1, &scissor);

}


void VulkanVideo::endRenderPass()
{
    vkCmdEndRenderPass(vkCommandBuffer);
}


VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;

void VulkanVideo::queueSubmit()
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &vkImageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = &waitDestStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &vkRenderingFinishedSemaphore;
    vkQueueSubmit(vkGraphicsQueue, 1, &submitInfo, vkFences[vkFrameIndex]);
}

void VulkanVideo::queuePresent()
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &vkRenderingFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkSwapchain;
    presentInfo.pImageIndices = &vkFrameIndex;
    vkQueuePresentKHR(vkPresentQueue, &presentInfo);

    //vkQueueWaitIdle(vkPresentQueue);
}



void VulkanVideo::destroy()
{
    for(uint32_t i = 0; i < vkSwapchainImageCount; ++i)
    {
        vkDestroyFence(vkDevice, vkFences[i], nullptr);
    }

    vkDestroySemaphore(vkDevice, vkRenderingFinishedSemaphore, nullptr);
    vkDestroySemaphore(vkDevice, vkImageAvailableSemaphore, nullptr);

    vkFreeCommandBuffers(vkDevice, vkCommandPool, vkCommandBuffers.size(), vkCommandBuffers.data());
    vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);

    for (size_t i = 0; i < vkSwapchainImageViews.size(); ++i)
    {
        vkDestroyFramebuffer(vkDevice, vkSwapchainFramebuffers[i], nullptr);
    }

    vkDestroyRenderPass(vkDevice, vkRenderPass, nullptr);
    vkFreeMemory(vkDevice, depthImageMemory, nullptr);
    vkDestroyImage(vkDevice, depthImage, nullptr);

    for (uint32_t i = 0; i < vkSwapchainImages.size(); ++i)
    {
        vkDestroyImageView(vkDevice, vkSwapchainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(vkDevice, vkSwapchain, nullptr);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);

}

//===================
VkSurfaceFormatKHR VulkanVideo::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}


VkPresentModeKHR VulkanVideo::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}


void VulkanVideo::createBuffer(VkDevice& device,
                            VkPhysicalDevice& physical,
                            VkDeviceSize size, 
                            VkBufferUsageFlags usage, 
                            VkMemoryPropertyFlags properties, 
                            VkBuffer& buffer, 
                            VkDeviceMemory& bufferMemory) 
{

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physical, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}


uint32_t VulkanVideo::findMemoryType(VkPhysicalDevice& physical, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physical, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}


VkBool32  VulkanVideo::getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat)
{
    std::vector<VkFormat> depthFormats = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };

    for (auto& format : depthFormats)
    {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            *depthFormat = format;
            return true;
        }
    }

    return false;

}


void VulkanVideo::createImage(VkDevice& device,
                           VkPhysicalDevice& physical,
                           uint32_t width,
                           uint32_t height,
                           VkFormat format,
                           VkImageTiling tiling,
                           VkImageUsageFlags usage,
                           VkMemoryPropertyFlags properties,
                           VkImage& image,
                           VkDeviceMemory& imageMemory)
{

    VkImageFormatProperties deviceProperties{};
    auto res = vkGetPhysicalDeviceImageFormatProperties(physical, format, VK_IMAGE_TYPE_2D, tiling, usage, 0, &deviceProperties);

    if (res != VK_SUCCESS)
    {
        throw std::runtime_error("bad image creation params!");
    }

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physical, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device, image, imageMemory, 0);
}

VkImageView VulkanVideo::createImageView(VkDevice& vkDevice, VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageView imageView;
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;


    if (vkCreateImageView(vkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void VulkanVideo::createSemaphore(VkSemaphore *semaphore)
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(vkDevice, &createInfo, nullptr, semaphore);
}

