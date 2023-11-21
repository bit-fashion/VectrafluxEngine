# ************************************************************************ #
#
# Copyright (C) 2022 Vincent Luo All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ************************************************************************ #

# Creates on 2022/9/14. #
cmake_minimum_required(VERSION 3.26)
project(VapourEngine)

set(CMAKE_CXX_STANDARD 23)

# Configure third party include of system.
include_directories(SYSTEM
        "${CMAKE_SOURCE_DIR}/Engine/Source/ThirdParty/vulkan/include"
        "${CMAKE_SOURCE_DIR}/Engine/Source/ThirdParty/glfw/include"
        "${CMAKE_SOURCE_DIR}/Engine/Source/ThirdParty/glad/include"
        "${CMAKE_SOURCE_DIR}/Engine/Source/ThirdParty/glm"
)

include_directories(
        "${CMAKE_SOURCE_DIR}/Engine/Source/Runtime"
)

# Configure link directories.
link_directories(
        "Engine/Source/ThirdParty/vulkan"
        "Engine/Source/ThirdParty/glfw/lib-mingw-w64"
)

add_executable(${PROJECT_NAME}
        "Engine/Source/Runtime/main.cpp"
        "Engine/Source/Runtime/Window/Window.cpp"
        "Engine/Source/Runtime/Render/RenderAPI.cpp"
        # Import glad src.
        "Engine/Source/ThirdParty/glad/src/glad.c"
)

target_link_libraries(${PROJECT_NAME}
        vulkan-1
        glfw3
)