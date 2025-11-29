cmake_minimum_required(VERSION 3.1)

project (cheekibreeki LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

set (CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
add_library(disarray STATIC IMPORTED)
set_target_properties(disarray PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../../build/libdisarray.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/../../include/"
)

add_executable(cheekibreeki
                src/Gopnik.cpp
                src/mainer.cpp
                src/Game.cpp
               )

target_link_libraries(cheekibreeki
                      disarray
                      SDL2
                      GL
                      openal
                      vulkan
                      ogg
                      vorbisfile
                      vorbis
                     )

