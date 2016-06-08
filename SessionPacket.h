#ifndef SESSIONPACKET_HPP
#define SESSIONPACKET_HPP
//
// Bridge class to Session. Extend Session implementation.
// Add specialisation through ideology to use packet in communication.
// writed by ecoretchi 02/08/2015
//
#include <ostream>
#include <string>
#include <asio.hpp>

#include "PacketParserBase.h"
#include "SessionBase.h"

class SessionPacket {
public:
    SessionPacket(SessionBase* session, PacketParserBase* pparser)
        : m_session(session),
          m_pparser(pparser){
    }
    SessionPacket(SessionPacket&) = delete;
public:
    bool sendPacket(const std::string& spacket) {
        if(!m_session)
            return false;
        asio::streambuf buf;
        std::ostream os(&buf);
        m_pparser->constructHeader(os, spacket);
        m_session->send(buf);
        m_session->send(spacket);
        return true;
    }
private:
    SessionBase* m_session;
    PacketParserBase* m_pparser;
};

#endif // SESSIONPACKET_HPP
