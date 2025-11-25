#pragma once

#ifdef _WIN32
    #ifdef _MSC_VER
        #include <alc.h>
        #include <al.h>
    #else
        #include <AL/alc.h>
        #include <AL/al.h>
    #endif
#else
    #include <AL/alc.h>
    #include <AL/al.h>
#endif


#include <vector>

#ifdef __ANDROID__
    class AAssetManager;
#endif

struct SoundData
{
    char name[255];
};


//----------

class SoundSystem{

    ALCdevice* alcdev;
    ALCcontext* alccont;

    ALuint* buffers;
    ALuint* sources;
    std::vector<SoundData> audioInfo;

#ifdef __ANDROID__
    char* LoadOGG(char *fileName,  ALsizei & size, ALenum &format, ALsizei &freq, AAssetManager* assman);
#else
    char* LoadOGG(char *fileName,  ALsizei & size, ALenum &format, ALsizei &freq);
#endif

public:

    static SoundSystem* getInstance()
    {
        static SoundSystem instance;
        return &instance;
    }

    bool init(ALCchar* dev);
#ifdef __ANDROID__
    bool loadFiles(const char* BasePath, const char* list, AAssetManager* assman);
#else
    bool loadFiles(const char* BasePath, const char* list);
#endif
    void setupListener(float * pos, float * orientation);
    void setSoundPos(unsigned int index, float * pos);
    void setVolume(unsigned int index, int volume);
    void playsound(unsigned int index, bool loop=false);
    void freeData();
    bool isPlaying(unsigned int index);
    ALuint getBuffer(unsigned int index){return buffers[index];}
    void stopAll();
    void exit();

private:

    SoundSystem(){}
    ~SoundSystem(){}
};
