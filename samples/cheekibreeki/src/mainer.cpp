#ifdef _WIN32
#ifdef  _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE 1

#pragma comment(lib,"disarray.lib")
#endif
#endif

#include <ctime>
#include "Game.h"
#include <disarray/SDLVideo.h>
#include <disarray/OSTools.h>
#include <disarray/disarray.h>


SDLVideo SDL;
Game game;

class DS : public disarray
{
public:
    DS(GameProto* game) : disarray(game){}
    void otherWhileLoopActivities() override {}
};


int main( int   argc, char *argv[] ){

    srand(time(0));

    char doc[255];
    GetHomePath(doc);
    sprintf(game.documentPath, "%s.cheekibreeki", doc);
    MakeDir(game.documentPath);

    char buf[1024];
    printf("Document path: %s\n", game.documentPath);
    sprintf(buf, "%s/settings.cfg", game.documentPath);


    DS ds(&game);
    ds.setupPC(&SDL, 480, 850, true, buf, "CHEEKI BREEKI", false);

    ds.runGamePC(&SDL);

    return 0;
}

