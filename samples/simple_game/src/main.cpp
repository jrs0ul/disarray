#ifdef _WIN32
#ifdef  _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE 1
#pragma comment(lib,"disarray.lib")
#endif
#endif

#include <disarray/SDLVideo.h>
#include <disarray/OSTools.h>
#include <disarray/disarray.h>
#include <ctime>
#include "Game.h"

SDLVideo SDL;
Game game;

class DS : public disarray
{
public:
    DS(GameProto* ng) : disarray(ng)
    {
    }
    void otherWhileLoopActivities() override {}
};

//--------------------
int main( int   argc, char *argv[] )
{
    srand(time(0));
    char buf[256];
    char finalBuf[1024];
    GetHomePath(buf);
    sprintf(game.documentPath, "%s.atari2600game", buf);
    MakeDir(game.documentPath);
    sprintf(finalBuf, "%s/settings.cfg", game.documentPath);

    DS ds(&game);

    ds.setupPC(&SDL, 640, 360, true, finalBuf, "ATARI 2600 game proto", false);
    ds.runGamePC(&SDL);

    return 0;
}
