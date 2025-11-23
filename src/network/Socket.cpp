#include <network/Socket.h>
#include <cstdio>
#ifndef _WIN32
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#endif

//--------------------------------------------

bool Socket::openAsServer(int port)
{
#ifdef _WIN32
    WSAStartup(MAKEWORD(2,0), &wsaData);
#endif
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock <= 0)
    {
        printf("Invalid socket!\n");
        return false;
    }

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port); // sets the socket the socket port


#ifdef _WIN32
    if (bind(sock,(LPSOCKADDR)&sin,sizeof(sockaddr_in)) == SOCKET_ERROR)
#else
    if (bind(sock, (sockaddr*)&sin, sizeof(sockaddr_in)) == -1)
#endif
    {
        printf("can't bind\n");
        return false;
    }

#ifdef _WIN32
    DWORD nonBlocking = 1;
    if ( ioctlsocket(sock,
                     FIONBIO,
                     &nonBlocking ) != 0 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }
#else

    int nonBlocking = 1;
    if ( fcntl(sock,
               F_SETFL,
               O_NONBLOCK,
               nonBlocking ) == -1 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }


#endif

    _open = true;
    return true;
}
//--------------------------------------
bool Socket::openAsClient()
{
#ifdef _WIN32
    WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock <= 0)
    {
        printf("Invalid socket!\n");
        return false;
    }


#ifdef _WIN32
    DWORD nonBlocking = 1;
    if ( ioctlsocket(sock,
                     FIONBIO,
                     &nonBlocking ) != 0 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }
#else

    int nonBlocking = 1;
    if ( fcntl(sock,
               F_SETFL,
               O_NONBLOCK,
               nonBlocking ) == -1 )
    {
        printf( "failed to set non-blocking\n" );
        return false;
    }


#endif


    _open = true;
    return _open;

}



//--------------------------------------

void Socket::shutdown()
{
    if (_open)
    {

#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif

#ifdef _WIN32
        WSACleanup();
#endif
        _open = false;
    }

}

//-----------------------------------
int Socket::getData(void *data, size_t dataSize, sockaddr_in& sender)
{
#ifdef _WIN32
    typedef int socklen_t;
#endif

    sockaddr_in sadd;
    socklen_t saddLength = sizeof(sadd);


    int bytes = recvfrom(sock, (char*)data, dataSize, 0, (sockaddr*)&sadd, &saddLength);

    if (bytes < 0)
    {
        return 0;
    }

    sender = sadd;

    return bytes;

}

//------------------------------------
void Socket::sendData( const void* data, int len, sockaddr_in& destination)
{

    sendto(sock, (const char*)data, len, 0, (sockaddr*)&destination, sizeof(sockaddr_in));

}
