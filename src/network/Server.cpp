#include <network/Server.h>
#include <cstdio>
#ifndef _WIN32
#include <arpa/inet.h>
#endif


Server::~Server()
{
    if (isRunning())
    {
        shutDown();
    }
}

//--------------------------------------------------------------
unsigned Server::clientCount()
{
    return connectedClientAddreses.count();
}

//---------------------------------------------------------------
void Server::shutDown()
{
    server.shutdown();
    receivedPackets.destroy();
    connectedClientAddreses.destroy();
}
//-------------------------------------------------------------------------
bool Server::launch(int port)
{
    return server.openAsServer(port);
}
//----------------------------------------------
void Server::getData()
{
    Message msg;
    memset(&msg.senderAddress, 0, sizeof(sockaddr_in));

    int len = server.getData(msg.data, MAX_MESSAGE_DATA_SIZE, msg.senderAddress);

    if (len)
    {

        msg.length = len;
        msg.parsed = false;
        receivedPackets.add(msg);

    }
}

//----------------------------------------------
void Server::sendData(unsigned clientIndex, const char *data, int len)
{

    if (clientIndex >= connectedClientAddreses.count())
    {
        printf("clientIndex %u does not exist\n", clientIndex);
        return;
    }

    sockaddr_in a;
    a.sin_family      = connectedClientAddreses[clientIndex].address.sin_family;
    a.sin_port        = connectedClientAddreses[clientIndex].address.sin_port;
    a.sin_addr.s_addr = connectedClientAddreses[clientIndex].address.sin_addr.s_addr;

    server.sendData(data, len, a);
}
//-------------------------------------------
void Server::removeClient(unsigned index)
{

    connectedClientAddreses.remove(index);

}

//-------------------------------------
bool Server::isRunning()
{
    return server.isOpen();
}
//-------------------------------------
void Server::addClient(const ClientFootprint& fp)
{
    connectedClientAddreses.add(fp);
}
//----------------------------------
Message* Server::fetchPacket(unsigned idx)
{
    if (idx < receivedPackets.count())
    {
        return &receivedPackets[idx];
    }

    return 0;
}
//-----------------------------------
void Server::discardPacket(unsigned idx)
{
    receivedPackets.remove(idx);
}
//-----------------------------------
int Server::findClientByAddress(const sockaddr_in& addr)
{

    for (unsigned i = 0; i < connectedClientAddreses.count(); ++i)
    {
        if ((connectedClientAddreses[i].address.sin_port == addr.sin_port) &&
            (connectedClientAddreses[i].address.sin_addr.s_addr == addr.sin_addr.s_addr))
        {
            return i;
        }
    }

    return -1;
}
