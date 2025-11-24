#pragma once

//Server class


#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#include "../DArray.h"
#include "Socket.h"
#include "Message.h"


struct ClientFootprint
{
    sockaddr_in address;
    unsigned ID;
    long lastMsgTimestamp;
};


class Server
{

    Socket server;
    DArray<ClientFootprint> connectedClientAddreses;

    DArray<Message> receivedPackets;

public:

    ~Server();

    unsigned clientCount();

    bool isRunning();

    void shutDown();

    bool launch(int port);

    void getData();

    void sendData(unsigned clientIndex, const char* data, int len);

    void addClient(const ClientFootprint& fp);
    void removeClient(unsigned index);

    unsigned storedPacketCount(){return receivedPackets.count();}
    Message* fetchPacket(unsigned idx);
    void discardPacket(unsigned idx);

    int findClientByAddress(const sockaddr_in& addr);
};
