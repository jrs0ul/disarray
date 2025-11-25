#ifndef __ogg_h__
#define __ogg_h__


#ifdef _WIN32
    #ifdef _MSC_VER
        #include <al.h>
    #else
        #include <AL/al.h>
    #endif
#else
    #include <AL/al.h>
#endif

#include <cstring>
#include <vorbis/vorbisfile.h>



#define BUFFER_SIZE (4096 * 8)
#define BUFFERCOUNT 8


#ifdef __ANDROID__
    class AAssetManager;
#endif

class OggStream
{

    FILE*           oggFile;
    OggVorbis_File  oggStream;
    vorbis_info*    vorbisInfo;

    ALuint buffers[BUFFERCOUNT];
    ALuint source;
    ALenum format;

    float volume;
    public:

    //opens stream from ogg file
    OggStream()
    {
        oggFile = nullptr;
        vorbisInfo = nullptr;
        source = 0;
        memset(&oggStream, 0, sizeof(OggVorbis_File));
        memset(buffers, 0 , sizeof(ALuint)*BUFFERCOUNT);
        volume = 0.2f;
    }

#ifdef __ANDROID__
    bool open(const char* path, AAssetManager* assman);
#else
    bool open(const char* path);
#endif
    void release();
    bool playback();
    bool playing();
    bool update();
    void stop();
    void setVolume(float vol);

    protected:

    long  stream(char* decbuff);
    void empty();
    void check(const char* place);
};


#endif // __ogg_h__
