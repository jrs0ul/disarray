#include <disarray/GameProto.h>
#include <disarray/audio/OggStream.h>
#include <disarray/audio/SoundSystem.h>
#include <disarray/SpriteBatcher.h>
#include <disarray/SysConfig.h>
#include <disarray/ShaderLoader.h>
#include <disarray/VulkanVideo.h>
#include <disarray/TouchData.h>

GameProto::GameProto()
{
    music   = new OggStream();
    pics    = new SpriteBatcher();
    sys     = new SystemConfig();
    shaders = new ShaderLoader();
    touches = new TouchData;

    vk = nullptr;
    screenWidth = 640;
    screenHeight = 480;
    works = true;
    windowed = false;

    dT = 1000.0f / 60.0f / 1000.0f;
    timeTicks = 0;
    accumulator = 0;
    tick = 0;

}

GameProto::~GameProto()
{
    delete touches;
    delete shaders;
    delete sys;
    delete pics;
    delete music;
}

void GameProto::destroy()
{
    printf("ProtoGame::destroy()\n");
    music->release();
    SoundSystem::getInstance()->freeData();
    SoundSystem::getInstance()->exit();

    printf("Destroing pics...\n");
    pics->destroy((vk) ? vk->getDevice() : nullptr);
    printf("Destroing shaders...\n");
    shaders->destroy();

}

void GameProto::loadConfig(const char* path,
                           uint32_t initialWidth,
                           uint32_t initialHeight,
                           bool     initialFullscreen,
                           int initialRenderIdx)
{
    sys->ScreenWidth = initialWidth;
    sys->ScreenHeight = initialHeight;
    sys->renderIdx = initialRenderIdx;
    sys->useWindowed = initialFullscreen;

    if (!sys->load(path))
    {
        sys->write(path);
    }

    screenWidth = sys->ScreenWidth;
    screenHeight = sys->ScreenHeight;
    windowed = sys->useWindowed;

}

int GameProto::fps (void)
{
    static int ctime = 0, _fps = 0, frames = 0, frames0 = 0;

    if ((int)timeTicks >= ctime) 
    {
        _fps = frames - frames0;
        ctime = (int)timeTicks + 1000;
        frames0 = frames;
    }

    frames = frames+1;
    return _fps;
}

