#include <disarray/network/Client.h>


bool Client::open()
{
    return client.openAsClient();
}

void Client::setServerAddress(const sockaddr_in& addr)
{
    serverAddress = addr;
}

void Client::getData()
{
    Message msg;
    memset(&msg.senderAddress, 0, sizeof(sockaddr_in));

    int len = client.getData(msg.data, MAX_MESSAGE_DATA_SIZE, msg.senderAddress);


    if ((msg.senderAddress.sin_port != serverAddress.sin_port) ||
            (msg.senderAddress.sin_addr.s_addr != serverAddress.sin_addr.s_addr))
    {
        return;
    }

    if (len)
    {

        msg.length = len;
        msg.parsed = false;
        receivedPackets.add(msg);

    }

}


void Client::sendData( const void* data, int len)
{
    client.sendData(data, len, serverAddress);
}

void Client::shutdown()
{
    receivedPackets.destroy();
    client.shutdown();
}


Message* Client::fetchPacket(unsigned idx)
{
    if (idx < receivedPackets.count())
    {
        return &receivedPackets[idx];
    }

    return 0;
}

void Client::discardPacket(unsigned idx)
{
    receivedPackets.remove(idx);
}
