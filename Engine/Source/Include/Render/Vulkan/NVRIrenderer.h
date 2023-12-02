/* ************************************************************************
 *
 * Copyright (C) 2022 Vincent Luo All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ************************************************************************/

/* Creates on 2022/11/23. */
#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <Engine.h>
#include <unordered_map>
#include <array>
#include <stddef.h>
#include <chrono>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class NatureWindow;

#define VK_LAYER_KHRONOS_validation "VK_LAYER_KHRONOS_validation"

#define vkNatureCreate(name, ...) \
    if (vkCreate##name(__VA_ARGS__) != VK_SUCCESS) \
        NATURE_THROW_ERROR("[VERIRRT ENGINE] [INIT_VULKAN_API] ERR/ - Create [{}] handle failed!", #name)
#define vkNatureAllocate(name, ...) \
    if (vkAllocate##name(__VA_ARGS__) != VK_SUCCESS) \
        NATURE_THROW_ERROR("[VERIRRT ENGINE] [INIT_VULKAN_API] ERR/ - Create [{}] handle failed!", #name)

/** GPU 设备信息 */
struct NVRIGPU {
    VkPhysicalDevice device;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
};

/** 队列族 */
struct NVRIQueueFamilyIndices {
    uint32_t graphicsQueueFamily = 0;
    uint32_t presentQueueFamily = 0;
};

/** SwapChain supports details struct. */
struct NVRISwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

/** 缓冲区结构体 */
struct NVRIbuffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;
};

/** 查询所有物理设备 */
static void NVRIGETGPU(VkInstance instance, std::vector<NVRIGPU> *pRIVGPU) {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, VK_NULL_HANDLE);
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, std::data(devices));

    for (const auto &device: devices) {
        NVRIGPU gpu{};
        gpu.device = device;
        /* 查询物理设备信息 */
        vkGetPhysicalDeviceProperties(device, &gpu.properties);
        /* 查询物理设备支持的功能列表 */
        vkGetPhysicalDeviceFeatures(device, &gpu.features);
        pRIVGPU->push_back(gpu);
    }
}

class NVRIswapchain;
class NVRIdevice;
class NVRIpipeline;

/** 顶点结构体 */
struct NVRIvertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
};

struct NVRItexture {
    VkImage image;
    VkImageView imageView;
    VkSampler sampler;
    VkFormat format;
    VkImageLayout layout;
    VkDeviceMemory memory;
};

struct NVRIUniformBufferObject {
    glm::mat4 m;
    glm::mat4 v;
    glm::mat4 p;
    float     t;
};

/**
 * 渲染管线
 */
class NVRIpipeline {
public:
    explicit NVRIpipeline(NVRIdevice *device, NVRIswapchain *swapchain,
                          const char *vertex_shader_path, const char *fragment_shader_path);
    ~NVRIpipeline();
    void Bind(VkCommandBuffer commandBuffer);
    void Write(VkDeviceSize offset, VkDeviceSize range, NVRIbuffer buffer, NVRItexture texture);

private:
    void Init_Graphics_Pipeline();

private:
    static VkVertexInputBindingDescription NVRIGetVertexInputBindingDescription() {
        VkVertexInputBindingDescription vertexInputBindingDescription = {};
        vertexInputBindingDescription.binding = 0;
        vertexInputBindingDescription.stride = sizeof(NVRIvertex);
        vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return vertexInputBindingDescription;
    };

    static std::array<VkVertexInputAttributeDescription, 3> NVRIGetVertexInputAttributeDescriptionArray() {
        std::array<VkVertexInputAttributeDescription, 3> array = {};
        /* position attribute */
        array[0].binding = 0;
        array[0].location = 0;
        array[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        array[0].offset = offsetof(NVRIvertex, position);

        /* color attribute */
        array[1].binding = 0;
        array[1].location = 1;
        array[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        array[1].offset = offsetof(NVRIvertex, color);

        array[2].binding = 0;
        array[2].location = 2;
        array[2].format = VK_FORMAT_R32G32_SFLOAT;
        array[2].offset = offsetof(NVRIvertex, texCoord);

        return array;
    }

private:
    NVRIdevice *mNVRIdevice;
    NVRIswapchain *mSwapchain;
    VkDescriptorSetLayout mUboDescriptorSetLayout;
    VkDescriptorSet mUboDescriptorSet;
    VkPipeline mPipeline;
    VkPipelineLayout mPipelineLayout;
};

/**
 * 交换链
 */
class NVRIswapchain {
public:
    NVRIswapchain(NVRIdevice *device, NatureWindow *pNatureWindow, VkSurfaceKHR surface);
    ~NVRIswapchain();

    VkResult AcquireNextImage(VkSemaphore semaphore, uint32_t *pIndex);

public:
    uint32_t GetWidth() { return mSwapchainExtent.width; }
    uint32_t GetHeight() { return mSwapchainExtent.height; }
    VkRenderPass GetRenderPass() { return mRenderPass; }
    uint32_t GetImageCount() { return mSwapchainImageCount; }
    VkFramebuffer GetFramebuffer(uint32_t index) { return mSwapchainFramebuffers[index]; }
    VkExtent2D GetExtent2D() { return mSwapchainExtent; }
    VkSwapchainKHR GetSwapchainKHRHandle() { return mSwapchain; }

private:
    void CreateSwapchain();
    void CleanupSwapchain();

private:
    NVRIdevice *mNVRIdevice;
    NatureWindow *mNatureWindow;
    VkSurfaceKHR mSurface = VK_NULL_HANDLE;
    VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    VkSurfaceFormatKHR mSurfaceFormatKHR;
    VkFormat mSwapchainFormat;
    VkExtent2D mSwapchainExtent;
    VkPresentModeKHR mSwapchainPresentModeKHR;
    uint32_t mSwapchainImageCount;
    std::vector<VkImage > mSwapchainImages;
    std::vector<VkImageView> mSwapchainImageViews;
    std::vector<VkFramebuffer> mSwapchainFramebuffers;
    NVRISwapChainSupportDetails mSwapChainDetails;
};

/**
 * 渲染设备（GPU）
 */
class NVRIdevice {
public:
    /* init and destroy function */
    explicit NVRIdevice(VkInstance instance, VkSurfaceKHR surface, NatureWindow *pNatureWindow);
    ~NVRIdevice();

    void CreateSwapchain(NVRIswapchain **pSwapchain);
    void DestroySwapchain(NVRIswapchain *swapchain);
    void CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> &bindings, VkDescriptorSetLayoutCreateFlags flags,
                                   VkDescriptorSetLayout *pDescriptorSetLayout);
    void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);
    void AllocateDescriptorSet(std::vector<VkDescriptorSetLayout> &descriptorSetLayouts, VkDescriptorSet *pDescriptorSet);
    void FreeDescriptorSet(uint32_t count, VkDescriptorSet *pDescriptorSet);
    void AllocateCommandBuffer(uint32_t count, VkCommandBuffer *pCommandBuffer);
    void FreeCommandBuffer(uint32_t count, VkCommandBuffer *pCommandBuffer);
    void CreateSemaphore(VkSemaphore *pSemaphore);
    void DestroySemaphore(VkSemaphore semaphore);
    void AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, NVRIbuffer *buffer);
    void FreeBuffer(NVRIbuffer buffer);
    void AllocateVertexBuffer(VkDeviceSize size, const NVRIvertex *pVertices, NVRIbuffer *pVertexBuffer);
    void AllocateIndexBuffer(VkDeviceSize size, const uint32_t *pIndices, NVRIbuffer *pIndexBuffer);
    void CopyBuffer(NVRIbuffer dest, NVRIbuffer src, VkDeviceSize size);
    void MapMemory(NVRIbuffer buffer, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData);
    void UnmapMemory(NVRIbuffer buffer);
    void WaitIdle();
    void BeginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usageFlags);
    void EndCommandBuffer(VkCommandBuffer commandBuffer);
    void SyncQueueSubmit(uint32_t commandBufferCount, VkCommandBuffer *pCommandBuffers,
                         uint32_t waitSemaphoreCount, VkSemaphore *pWaitSemaphores,
                         uint32_t signalSemaphoreCount, VkSemaphore *pSignalSemaphores,
                         VkPipelineStageFlags *pWaitDstStageMask);
    void BeginOneTimeCommandBufferSubmit(VkCommandBuffer *pCommandBuffer);
    void EndOneTimeCommandBufferSubmit();
    void CreateTexture(const char *path, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, NVRItexture *pTexture);
    void DestroyTexture(NVRItexture texture);
    void TransitionTextureLayout(NVRItexture *texture, VkImageLayout newLayout);
    void CopyTextureBuffer(NVRIbuffer buffer, NVRItexture texture, uint32_t width, uint32_t height);

public:
    VkPhysicalDevice GetPhysicalDeviceHandle() { return mNVRIGPU.device; }
    VkDevice GetDeviceHandle() { return mDevice; }
    VkQueue GetGraphicsQueue() { return mGraphicsQueue; }
    VkQueue GetPresentQueue() { return mPresentQueue; }

private:
    void InitAllocateDescriptorSetPool();
    void InitCommandPool();
    NVRIQueueFamilyIndices FindQueueFamilyIndices();

private:
    VkInstance mInstance = VK_NULL_HANDLE;
    VkDevice mDevice = VK_NULL_HANDLE;
    VkSurfaceKHR mSurfaceKHR = VK_NULL_HANDLE;
    NVRIGPU mNVRIGPU = {};
    NatureWindow *mNatureWindow;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
    VkMemoryRequirements mMemoryRequirements;
    VkCommandBuffer mSingleTimeCommandBuffer = VK_NULL_HANDLE;
    /* 队列族 */
    NVRIQueueFamilyIndices mNVRIQueueFamilyIndices;
    VkQueue mGraphicsQueue = VK_NULL_HANDLE;
    VkQueue mPresentQueue = VK_NULL_HANDLE;
    /* 设备支持的扩展列表 */
    std::unordered_map<std::string, VkExtensionProperties> mDeviceSupportedExtensions;
};

/**
 * 渲染硬件接口
 */
class NVRIrenderer {
public:
    explicit NVRIrenderer(NatureWindow *pNatureWindow);
    ~NVRIrenderer();

    /* Render interface. */
    void BeginRender();
    void Draw();
    void EndRender();

private:
    void Init_Vulkan_Impl();
    void CleanupSwapchain();
    void CreateSwapchain();
    void RecreateSwapchain();
    void BeginRecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);
    void EndRecordCommandBuffer();
    void BeginRenderPass(VkRenderPass renderPass);
    void EndRenderPass();
    void QueueSubmitBuffer();
    void UpdateUniformBuffer();

private:
    const std::vector<NVRIvertex> mVertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f,  0.5f,  0.0f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f,  0.0f},  {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };
    const std::vector<uint32_t> mIndices = {
            0, 1, 2, 2, 3, 0
    };
    NVRIbuffer mVertexBuffer;
    NVRIbuffer mIndexBuffer;
    NVRIbuffer mUniformBuffer;
    NVRItexture mTexture;
    VkInstance mInstance = VK_NULL_HANDLE;
    VkSurfaceKHR mSurface = VK_NULL_HANDLE;
    NatureWindow *mNatureWindow;
    std::unique_ptr<NVRIdevice> mNVRIdevice = NULL;
    std::unique_ptr<NVRIpipeline> mNVRIpipeline = NULL;
    NVRIswapchain *mNVRIswapchain = NULL;
    VkSemaphore mImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore mRenderFinishedSemaphore = VK_NULL_HANDLE;
    /* Vectors. */
    std::vector<const char *> mRequiredInstanceExtensions;
    std::vector<const char *> mRequiredInstanceLayers;
    std::vector<VkCommandBuffer> mCommandBuffers;
    /* Map */
    std::unordered_map<std::string, VkExtensionProperties> mVkInstanceExtensionPropertiesSupports;
    std::unordered_map<std::string, VkLayerProperties> mVkInstanceLayerPropertiesSupports;
    /* Context */
    VkCommandBuffer mCurrentContextCommandBuffer = VK_NULL_HANDLE;
    VkRenderPass mCurrentContextRenderPass = VK_NULL_HANDLE;
    uint32_t mCurrentContextImageIndex = 0;
};