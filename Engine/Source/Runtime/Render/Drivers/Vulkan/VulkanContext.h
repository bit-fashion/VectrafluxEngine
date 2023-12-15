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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, e1ither express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ************************************************************************/

/* Creates on 2022/9/14. */

/*
  ===================================
    @author bit-fashion
  ===================================
*/
#ifndef _SPORTS_VULKAN_CONTEXT_H_
#define _SPORTS_VULKAN_CONTEXT_H_

#include <vulkan/vulkan.h>
#include <Typedef.h>
#include <Engine.h>
#include <stdexcept>

class Window;

class VulkanContext {
public:
    struct DriverPhysicalDevice {
        VkPhysicalDevice device;
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
    };

public:
    VulkanContext(const Window *pWindow);
   ~VulkanContext();

private:
    void InitVulkanDriverContext();

private:
    VkInstance m_Instance;
    VkSurfaceKHR m_SurfaceKHR;
    VkDevice m_Device;

    const Window *m_Window;
    DriverPhysicalDevice m_DriverPhysicalDevice;
};

#endif /* _SPORTS_VULKAN_CONTEXT_H_ */
