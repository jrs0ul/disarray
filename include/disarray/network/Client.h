#pragma once

#include "../DArray.h"
#include "Socket.h"
#include "Message.h"

class Client
{
    Socket client;
    sockaddr_in serverAddress;
    DArray<Message> receivedPackets;

public:

    Client():
    client()
    {
        memset(&serverAddress, 0, sizeof(sockaddr_in));
    }

    bool open();
    void setServerAddress(const sockaddr_in& addr);
    bool isOpen(){return client.isOpen();}
    void getData();
    void sendData( const void* data, int len);
    void shutdown();


    unsigned storedPacketCount(){return receivedPackets.count();}
    Message* fetchPacket(unsigned idx);
    void discardPacket(unsigned idx);

};
