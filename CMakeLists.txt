cmake_minimum_required(VERSION 3.1)

project(disarray LANGUAGES CXX)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(disarray PRIVATE "include/")

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    include_directories("${OPENAL_DIR}/include")
    include_directories("${OGG_VORBIS_DIR}/inc")
endif()

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
        src/AndroidFile.cpp
        src/ShaderLoader.cpp
        src/GameProto.cpp

)
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set_source_files_properties(src/Extensions.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
    set_source_files_properties(src/SDLVideo.cpp PROPERTIES HEADER_FILE_ONLY TRUE)


    add_library(openal-soft SHARED IMPORTED)
    set_target_properties(openal-soft PROPERTIES IMPORTED_LOCATION
            "${OPENAL_DIR}/${ANDROID_ABI}/libopenal.so")
    add_library(ogg SHARED IMPORTED)
    set_target_properties(ogg PROPERTIES IMPORTED_LOCATION
            "${OGG_VORBIS_DIR}/lib/${ANDROID_ABI}/libogg.a")
    add_library(vorbis SHARED IMPORTED)
    set_target_properties(vorbis PROPERTIES IMPORTED_LOCATION
            "${OGG_VORBIS_DIR}/lib/${ANDROID_ABI}/libvorbis.a")
else()
    set_source_files_properties(src/AndroidFile.cpp PROPERTIES HEADER_FILE_ONLY TRUE)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(disarray PUBLIC
            android
            log
            vulkan
            openal-soft
            EGL
            GLESv2
            ogg
            vorbis
    )

else ()
    target_link_libraries(disarray
            SDL2
            GL
            openal
            vorbisfile
            ogg
            vulkan
    )
endif ()


