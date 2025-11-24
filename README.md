# DISARRAY

A collection of crappy C/C++ code that helps me make games for **Linux**,
**Windows** and **Android**.

### Libraries you will need for this to work
- SDL2
- OpenAL(OpenAL-soft)
- Vulkan
- Ogg
- Vorbis
  
This article can help you to build OpenAL for Android:

https://digitalkarabela.com/how-to-compile-openal-library-for-android-with-oboe-library/

Prebuilt Ogg/Vorbis for Android:

https://github.com/mregnauld/ogg-vorbis-libraries-android

### How to build:

```
mkdir build
cd build
cmake ..
cmake --build . -- -j8
```
The easiest way is to put everything inside your project and add this line to your main CMakeLists.txt:
```
add_subdirectory("/path/to/disarray")
```
For Android you need to put paths to OpenAL-soft and Ogg/Vorbis libs into your build.gradle:
```
 defaultConfig {
        ...
        externalNativeBuild {
            cmake {
                ...
                arguments ...
                          '-DOPENAL_DIR=/path/to/libopenal',
                          '-DOGG_VORBIS_DIR=/path/to/oggvorbis'
            }
        }
    }
```
