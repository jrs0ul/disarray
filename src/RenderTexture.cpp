#include <disarray/RenderTexture.h>
#ifndef __ANDROID__
    #include <disarray/Extensions.h>
#else
    #include <android/log.h>
    #include <android/log_macros.h>
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#endif
#include <disarray/VulkanVideo.h>


void RenderTexture::create(uint32_t width,
                           uint32_t height,
                           uint8_t filter,
                           bool isVulkan,
                           VkDevice* device,
                           VkPhysicalDevice* physical)
{
    useVulkan = isVulkan;
    _width = width;
    _height = height;

#ifdef __ANDROID__
    LOGI(
#else
    printf(
#endif
            "creating render texture %ux%u\n", _width, _height);

    if (!isVulkan)
    {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glGenTextures(1, &fboTexture);
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter == 1 ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter == 1 ? GL_LINEAR : GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else  // VULKAN
    {
        VulkanVideo::createImage(*device,
                              *physical,
                              _width,
                              _height,
                              VK_FORMAT_R8G8B8A8_UNORM,
                              VK_IMAGE_TILING_OPTIMAL,

                              VK_IMAGE_USAGE_SAMPLED_BIT |
                              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                              vkImage,
                              vkTextureMemory);

        vkImageView = VulkanVideo::createImageView(*device,
                                                vkImage,
                                                VK_FORMAT_R8G8B8A8_UNORM,
                                                VK_IMAGE_ASPECT_COLOR_BIT);


        VkAttachmentDescription colorAttachment{};
        colorAttachment.format         = VK_FORMAT_R8G8B8A8_UNORM;
        colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments    = &colorAttachment;
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subpass;

        VkSubpassDependency dependency{};
        dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass          = 0;
        dependency.srcStageMask        = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependency.srcAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
        dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                         VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags     = VK_DEPENDENCY_BY_REGION_BIT;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        vkCreateRenderPass(*device, &renderPassInfo, nullptr, &vkRenderPass);
        //---

        VkImageView attachments[] = {vkImageView};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = vkRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = attachments;
        framebufferInfo.width           = _width;
        framebufferInfo.height          = _height;
        framebufferInfo.layers          = 1;

        vkCreateFramebuffer(*device, &framebufferInfo, nullptr, &vkFB);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        VkFilter vkfilter = VK_FILTER_NEAREST;

        samplerInfo.magFilter = vkfilter;
        samplerInfo.minFilter = vkfilter;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

        vkCreateSampler(*device, &samplerInfo, nullptr, &vkSampler);

    }
}

void RenderTexture::bind(VkCommandBuffer* vkCmd)
{
    if (!useVulkan)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, _width, _height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else //VULKAN
    {
        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass        = vkRenderPass;
        render_pass_info.framebuffer       = vkFB;
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent.width = _width;
        render_pass_info.renderArea.extent.height = _height;

        std::vector<VkClearValue> clearValues(1);
        clearValues[0].color = {0,0,0,0};

        render_pass_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
        render_pass_info.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(*vkCmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)_width;
        viewport.height = (float)_height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
       
        vkCmdSetViewport(*vkCmd, 0, 1, &viewport);

        const VkRect2D scissor = {{0, 0}, {_width, _height}};
        vkCmdSetScissor(*vkCmd, 0, 1, &scissor);
    }

}

void RenderTexture::unbind(VkCommandBuffer* vkCmd)
{
    if (!useVulkan)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else //VULKAN
    {
        vkCmdEndRenderPass(*vkCmd);
    }
}


void RenderTexture::getVulkanTexture(VulkanTexture& tex)
{
    tex.vkImage         = vkImage;
    tex.vkImageView     = vkImageView;
    tex.vkTextureMemory = vkTextureMemory;
    tex.vkSampler       = vkSampler;
}


void RenderTexture::destroy()
{
    if (!useVulkan)
    {
        glDeleteFramebuffers(1, &fbo);
    }
}
