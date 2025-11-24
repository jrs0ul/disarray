#include <audio/SoundSystem.h>

#include <cstdio>
#include <cwchar>
#include <vorbis/vorbisfile.h>

#ifdef __ANDROID__
#include <android/log.h>
#include <AndroidFile.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#endif

#include <Xml.h>


//-------------------------------------------------------------------
#ifdef __ANDROID__
char* SoundSystem::LoadOGG(char *fileName,  ALsizei & size,
                           ALenum &format, ALsizei &freq, AAssetManager* assman)
#else
char* SoundSystem::LoadOGG(char *fileName,  ALsizei & size,
                           ALenum &format, ALsizei &freq)
#endif
{

    char * buffer = nullptr;
    const int BUFFER_SIZE = 16384 ;
    int endian = 0;

    long bytes;
    char data[BUFFER_SIZE];

    FILE* f = nullptr;
#ifdef __ANDROID__

    f = android_fopen(assman, fileName, "rb");
#else
    f = fopen(fileName, "rb");
#endif

    if (!f)
    {
        printf("Cannot open %s\n",fileName);
        return 0;
    }

    vorbis_info *pInfo;
    OggVorbis_File oggFile;

    if (ov_open(f, &oggFile, NULL, 0) != 0){
        printf("Error while opening %s\n",fileName);
        return 0;
    }

    pInfo = ov_info(&oggFile, -1);

    if (pInfo->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;

    freq = pInfo->rate;

    bytes = 1;
    buffer = 0;
    size = 0;
    int siz =0;
    int btmp = 1;
    while (bytes > 0){

        //fill the buffer
        siz = 0;
        while ((btmp) && (siz<BUFFER_SIZE)){

            btmp = ov_read(&oggFile, data+siz, BUFFER_SIZE-siz, endian, 2, 1, 0);
            siz+=btmp;
        }

        bytes = siz;
        if (bytes< 0){
            ov_clear(&oggFile);
            printf("Error while decoding %s\n",fileName);
            return 0;
        }
        printf("#");


        char * tmp = 0;
        if (size){
            tmp = new char[size+1];
            memcpy(tmp,buffer,size);
            delete []buffer;
        }
        size+=bytes;
        buffer=new char[size];
        memcpy(buffer,tmp,size-bytes);
        memcpy(&buffer[size-bytes],data,bytes);
        if (tmp){
            delete []tmp;
        }


    }

    ov_clear(&oggFile);
    puts("__");
    return buffer;
    }


//---------------------------------------------------
bool SoundSystem::init(ALCchar* dev){

    alcdev=0; buffers=0; sources=0; 

    int r;

    alGetError();

    if (dev)
        printf("Using %s as sound device...\n", dev );


    alcdev = alcOpenDevice(dev);


    if (!alcdev){

        r = alGetError();
        if ( r != AL_NO_ERROR){
            printf("Error: %x while initialising SoundSystem\n",r);
        }
        return false;
    }

    alccont=alcCreateContext(alcdev,0);
    alcMakeContextCurrent(alccont);


    r = alGetError();
    if ( r != AL_NO_ERROR){
        printf("Error: %x while creating context\n",r);
        return false;
    }

    return true;
}
//---------------------------------------
void SoundSystem::exit(){
    if (alcdev){
        alccont=alcGetCurrentContext();
        alcdev=alcGetContextsDevice(alccont);
        alcMakeContextCurrent(0);
        alcDestroyContext(alccont);

        alcCloseDevice(alcdev);
    }
}
//--------------------------------------
#ifdef __ANDROID__
void SoundSystem::loadFiles(const char *BasePath, const char *list, AAssetManager* assman)
#else
void SoundSystem::loadFiles(const char *BasePath, const char *list)
#endif
{
    char buf[255];
    sprintf(buf, "%s%s", BasePath, list);

    Xml sfxList;

#ifndef __ANDROID__
    bool result = sfxList.load(buf);
#else
    bool result = sfxList.load(buf, assman);
#endif

    if (result)
    {
        XmlNode *mainnode = sfxList.root.getNode(L"Sounds");

        int soundCount = 0;

        if (mainnode)
        {
            soundCount = mainnode->childrenCount();
        }

        for (int i = 0; i < soundCount; ++i)
        {
            XmlNode *node = mainnode->getNode(i);

            if (node)
            {
                for (int j = 0; j < (int)node->attributeCount(); ++j)
                {
                    XmlAttribute* attr = node->getAttribute(j);
                    SoundData data;
                    data.name[0] = 0;

                    if (attr)
                    {
                        if (wcscmp(attr->getName(), L"src") == 0)
                        {
                            wchar_t* value = attr->getValue();

                            if (value)
                            {
                                sprintf(data.name, "%ls", value);
                                audioInfo.push_back(data);
                            }
                        }
                    }
                }
            }
        }

        sfxList.destroy();
    }

    buffers = new ALuint[audioInfo.size()];
    alGenBuffers(audioInfo.size(), buffers);

    sources = new ALuint[audioInfo.size()];
    alGenSources(audioInfo.size(), sources);

    char *data = nullptr;
    ALenum format;
    ALsizei size, freq;
    //  ALboolean loop;


    for (unsigned int i = 0; i < audioInfo.size(); i++)
    {

        sprintf(buf, "%s%s", BasePath, audioInfo[i].name);

#ifdef __ANDROID__
        data = LoadOGG(buf, size, format, freq, assman);
#else
        data = LoadOGG(buf, size, format, freq);
#endif

        if (data)
        {
            alBufferData(buffers[i], format, data, size, freq);
            delete[]data;
            data = nullptr;
        }
        else
        {
#ifdef __ANDROID__
            LOGI("Error loading %s", buf);
#else
            puts("No data");
#endif
        }
        alSourcei(sources[i], AL_BUFFER, buffers[i]);
        alSource3f(sources[i], AL_POSITION, 0.0, 0.0, 0.0);
        alSource3f(sources[i], AL_VELOCITY, 0.0, 0.0, 0.0);
        alSource3f(sources[i], AL_DIRECTION, 0.0, 0.0, 0.0);
        alSourcef(sources[i], AL_ROLLOFF_FACTOR, 0.0);
        alSourcei(sources[i], AL_SOURCE_RELATIVE, AL_TRUE);
    }

}

//--------------------------------------------
void SoundSystem::playsound(unsigned int index, bool loop)
{

    if (index < audioInfo.size()){
        if (loop)
            alSourcei(sources[index],AL_LOOPING,AL_TRUE);
        else
            alSourcei(sources[index],AL_LOOPING,AL_FALSE);

        alSourcePlay(sources[index]);
    }

}
//--------------------------------------------

void SoundSystem::freeData(){

    if (audioInfo.size())
    {
        alDeleteSources(audioInfo.size(), sources);
        alDeleteBuffers(audioInfo.size(), buffers);
        delete []buffers;
        delete []sources;
        buffers = nullptr;
        sources = nullptr;
    }
    audioInfo.clear();
}

//----------------------------------------
bool SoundSystem::isPlaying(unsigned int index){


    ALenum state;

    alGetSourcei(sources[index], AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);

}

//-------------------------------------------
void SoundSystem::setupListener(float * position, float * orientation){
    alListener3f(AL_POSITION,position[0], position[2], position[1]);
    alListenerf(AL_GAIN, 1.0f);
    alListener3f(AL_ORIENTATION,orientation[0], orientation[2], orientation[1]);
}
//------------------------------------
void SoundSystem::setSoundPos(unsigned int index, float* pos)
{
    alSourcefv(sources[index], AL_POSITION, pos);
}
//--------------------------------------
void SoundSystem::setVolume(unsigned int index, int volume)
{
    alSourcef(sources[index], AL_GAIN, volume);
}

//---------------------------------------------
void SoundSystem::stopAll(){
    alSourceStopv(audioInfo.size(), sources);
}
