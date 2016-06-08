#ifndef PACKETPARSERBASE_H
#define PACKETPARSERBASE_H
//
// SessionPacket using this interface, and PacketParser implement it
// writed by ecoretchi 02/08/2015
//
#include <string>
#include <ostream>

class PacketParserBase
{
public:
    virtual void constructHeader(std::ostream& ostr, const std::string& spacket) =0;
};

#endif // PACKETPARSERBASE_H
