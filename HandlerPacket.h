#ifndef HANDLERPACKET_H
#define HANDLERPACKET_H
//
// Packages communication guarantee a whole packet
// at notification method onPacketReceived will come
// writed by ecoretchi 02/08/2015
//
#include <string>
class SessionPacket;
class HandlerPacket
{
public:
    virtual void onSessionStarted(SessionPacket *) = 0;
    virtual void onPacketReceived(const std::string&) = 0;
};

#endif // HANDLERPACKET_H
