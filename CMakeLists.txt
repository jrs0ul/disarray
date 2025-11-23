cmake_minimum_required(VERSION 3.1)

project(disarray LANGUAGES CXX)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(disarray PRIVATE "include/")

add_library(disarray
    src/gui/SelectMenu.cpp
    src/OSTools.cpp
    src/Particles2D.cpp
    src/gui/Text.cpp
    src/gui/Slider.cpp
    src/gui/EditBox.cpp
    src/SysConfig.cpp
    src/network/Socket.cpp
    src/network/Client.cpp
    src/network/Server.cpp
    src/Vectors.cpp
    src/Xml.cpp
    src/Matrix.cpp
    src/Useful.cpp
    src/SpriteBatcher.cpp
    src/Image.cpp
    src/RenderTexture.cpp
    src/VulkanVideo.cpp
    src/SDLVideo.cpp
    src/Extensions.cpp
    src/ShaderProgram.cpp
    src/Shader.cpp
    src/audio/OggStream.cpp
    src/audio/SoundSystem.cpp

    )

target_link_libraries(disarray
    SDL2
    GL
    openal
    vorbisfile
    ogg
    vulkan
    )

