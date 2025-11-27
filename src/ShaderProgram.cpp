/*
 The Disarray 
 by jrs0ul(jrs0ul ^at^ gmail ^dot^ com) 2025
 -------------------------------------------
 Shader program
 */

#include <disarray/ShaderProgram.h>
#include <cstdio>
#include <disarray/VulkanVideo.h>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#endif

void ShaderProgram::create(bool useVulkan)
{

    isVulkanShader = useVulkan;

    if (!useVulkan)
    {
        program = glCreateProgram();
    }
}

void ShaderProgram::destroy(VkDevice* vkDevice)
{
    if (!isVulkanShader)
    {
        glDeleteProgram(program);
    }
    else // VULKAN
    {
        printf("Deleting vulkan pipeline, shaders and buffers...\n");

        vkDestroyPipeline(*vkDevice, vkPipeline, nullptr);
        vkDestroyPipelineLayout(*vkDevice, vkPipelineLayout, nullptr);

        for (uint32_t i = 0; i < vkShaderStages.size(); ++i)
        {
            vkDestroyShaderModule(*vkDevice, vkShaderStages[i].module, nullptr);
        }


        for (int i = 0; i < VULKAN_BUFFER_COUNT; ++i)
        {
            vkUnmapMemory(*vkDevice, vkVertexBuffersMemory[i]);
            vkFreeMemory(*vkDevice, vkVertexBuffersMemory[i], nullptr);
            vkDestroyBuffer(*vkDevice, vkVertexBuffers[i], nullptr);
        }
    }
}

void ShaderProgram::getLog(char* string, int len)
{
#ifndef __ANDROID__
    glGetInfoLogARB(program, len, 0, string);
#else
    glGetShaderInfoLog(program, len, 0, string);
#endif
}

void ShaderProgram::attach(Shader & sh)
{
    if (!isVulkanShader)
    {
        glAttachShader(program, sh.id);
    }
    else //VULKAN
    {
        VkPipelineShaderStageCreateInfo vkShaderStage{};
        vkShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vkShaderStage.pNext = nullptr;
        vkShaderStage.stage = (sh.type == VERTEX_SHADER) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        vkShaderStage.module = sh.vkShaderModule;
        vkShaderStage.pName = "main";


        vkShaderStages.push_back(vkShaderStage);
    }
}

void ShaderProgram::link()
{
    glLinkProgram(program);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
    {
#ifdef __ANDROID__
        LOGI("Linking failed!");
#else
        printf("Linking failed!!!1\n");
#endif

    }
    else
    {
        //printf("All good\n");
    }
}


void ShaderProgram::buildVkPipeline(VkDevice* device,
                                    VkPhysicalDevice* physical,
                                    VkRenderPass* pass,
                                    VkDescriptorSetLayout* dsl,
                                    bool needUvs,
                                    bool needAlphaBlend)
{
    const int MAX_VERTEX_BUFFER_SIZE = sizeof(float) * 4 * 100000;

    for (int i = 0; i < VULKAN_BUFFER_COUNT; ++i)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = MAX_VERTEX_BUFFER_SIZE;
        bufferInfo.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(*device, &bufferInfo, nullptr, &vkVertexBuffers[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*device, vkVertexBuffers[i], &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanVideo::findMemoryType(*physical, memRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(*device, &allocInfo, nullptr, &vkVertexBuffersMemory[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(*device, vkVertexBuffers[i], vkVertexBuffersMemory[i], 0);


        vkMapMemory(*device,
                    vkVertexBuffersMemory[i],
                    0,
                    VK_WHOLE_SIZE,
                    0,
                    &vkMappedBuffer[i]);
    }


    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 64;



    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.flags = 0;

    if (dsl)
    {
        pipelineLayoutInfo.pSetLayouts = dsl;
    }

    pipelineLayoutInfo.setLayoutCount = (dsl) ? 1 : 0;

    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    vkCreatePipelineLayout(*device, &pipelineLayoutInfo, nullptr, &vkPipelineLayout);


    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;


    const int bindingCount = (needUvs) ? 3 : 2;

    for (int i = 0; i < bindingCount; ++i)
    {
        VkVertexInputBindingDescription b = {};
        b.binding = i;
        b.stride = 8;

        if ((needUvs && i == 2) || (!needUvs && i == 1)) //for colors
        {
            b.stride = 16;
        }

        b.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindings.push_back(b);
    }


    //pos
    VkVertexInputAttributeDescription positionAttribute = {};
    positionAttribute.binding = 0;
    positionAttribute.location = 0;
    positionAttribute.format = VK_FORMAT_R32G32_SFLOAT;
    positionAttribute.offset = 0;

    //uvs
    VkVertexInputAttributeDescription uvsAttribute = {};
    uvsAttribute.binding = 1;
    uvsAttribute.location = 1;
    uvsAttribute.format = VK_FORMAT_R32G32_SFLOAT;
    uvsAttribute.offset = 0;

    //Color
    VkVertexInputAttributeDescription colorAttribute = {};
    colorAttribute.binding = (needUvs) ? 2 : 1;
    colorAttribute.location = (needUvs) ? 2 : 1;
    colorAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    colorAttribute.offset = 0;

    attributes.push_back(positionAttribute);

    if (needUvs)
    {
        attributes.push_back(uvsAttribute);
    }

    attributes.push_back(colorAttribute);


    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.pVertexBindingDescriptions = bindings.data();
    vertexInputInfo.vertexBindingDescriptionCount = bindings.size();
    vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
    vertexInputInfo.vertexAttributeDescriptionCount = attributes.size();


    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.pNext = nullptr;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;


    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.pNext = nullptr;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    //rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    //rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo   multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.pNext = nullptr;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    if (needAlphaBlend)
    {
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    VkPipelineColorBlendStateCreateInfo    colorBlending = {};
    colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext           = nullptr;
    colorBlending.logicOpEnable   = VK_FALSE;
    colorBlending.logicOp         = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments    = &colorBlendAttachment;

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    depthStencilInfo.depthTestEnable = VK_FALSE; // Enable depth testing
    depthStencilInfo.depthWriteEnable = VK_FALSE; // Enable writing to depth buffer
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS; // Use less comparison
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE; // Disable depth bounds testing
    depthStencilInfo.stencilTestEnable = VK_FALSE; // Disable stencil testing
    depthStencilInfo.minDepthBounds = 0.0f; // Optional
    depthStencilInfo.maxDepthBounds = 1.0f; // Optional


    const VkDynamicState dynamicStates[2] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo;
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.pNext = nullptr;
    dynamicInfo.flags = 0;
    dynamicInfo.dynamicStateCount = 2;
    dynamicInfo.pDynamicStates = dynamicStates;
    

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext               = nullptr;
    pipelineInfo.stageCount          = vkShaderStages.size();
    pipelineInfo.pStages             = vkShaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.layout              = vkPipelineLayout;
    pipelineInfo.pDynamicState       = &dynamicInfo;
    pipelineInfo.renderPass          = *pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.pDepthStencilState = &depthStencilInfo;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(*device,
                                  VK_NULL_HANDLE,
                                  1,
                                  &pipelineInfo,
                                  nullptr,
                                  &vkPipeline) != VK_SUCCESS)
    {
        printf("Failed to create a pipeline\n");
    }


}

void ShaderProgram::use(VkCommandBuffer* vkCmd)
{
    if (!isVulkanShader)
    {
        glUseProgram(program);
    }
    else // VULKAN
    {
        vkCmdBindPipeline(*vkCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
    }
}

int ShaderProgram::getUniformID(const char* name)
{
    return  glGetUniformLocation(program, name);
}

int ShaderProgram::getAttributeID(const char* name)
{
    return glGetAttribLocation(program, name);
}

void ShaderProgram::pushConstants(std::vector<DUniform>& constants, VkCommandBuffer* vkCmd)
{
    if (isVulkanShader)
    {
        uint8_t bigBuf[255];

        size_t size = 0;
        for (auto& con : constants)
        {
            memcpy(&bigBuf[size], con.data, con.size);
            size += con.size;
        }

        vkCmdPushConstants(*vkCmd, *getVkPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, size, bigBuf);
    }
}

void ShaderProgram::updateUniforms(std::vector<DUniform>& uniforms)
{
    for ( auto& uni : uniforms)
    {
        if (!isVulkanShader)
        {
            int loc = glGetUniformLocation(program, uni.name);
            int nameLen = 0;
            int dataSize = 0;
            GLenum type;
            glGetActiveUniform(program, loc, MAX_UNIFORM_NAME, &nameLen, &dataSize, &type, uni.name);

            switch (type)
            {
                case GL_FLOAT_MAT4 : glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)uni.data); break;
                case GL_FLOAT : {float tim = 0; memcpy(&tim, uni.data, sizeof(float)); glUniform1f(loc, tim);} break;
                /*case GL_FLOAT_VEC2 : glUniform2f(loc, 1, (float*)uni.data); break;
                case GL_FLOAT_VEC3 : glUniform3f(loc, 1, (float*)uni.data); break;
                case GL_FLOAT_VEC4 : glUniform4f(loc, 1, (float*)uni.data); break;*/
            }
        }
        else
        {

        }
    }
}
