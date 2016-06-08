#ifndef HandlerPacketParser_HPP
#define HandlerPacketParser_HPP
//
// Proxy from HandlerRaw interface to HandlerPacket interface
// writed by ecoretchi 02/08/2015
//

#include <iostream>
#include <string>
#include <asio.hpp>
#include "logtrace.h"
#include "variableconverter.h"

#include "HandlerRaw.h"
#include "SessionBase.h"
#include "PacketParser.hpp"
#include "SessionPacket.h"

template<class THandlerPacket=HandlerPacket>
class HandlerPacketParser : public HandlerRaw
{
public:
    HandlerPacketParser():m_session(0),m_psession(0){
        m_hpacket = new THandlerPacket;
        m_pparser = new PacketParser<THandlerPacket>(m_hpacket);
    }
public:
    virtual std::string name() const override {
        return "PacketDataHandle";
    }
public:
    virtual void onSessionStarted(SessionBase *session) override {
        if(m_session  || m_psession)
            throw "session already exist, have to use empty handler resource";
        m_session = session;
        m_psession = new SessionPacket(m_session, m_pparser);
        m_hpacket->onSessionStarted(m_psession);
    }
    virtual void parsing(std::istream &istrm, SessionBase *session) override {
        size_t sz = session->streamBufferSize();
        m_pparser->parsing(istrm, sz);
    }
protected:
    SessionPacket* sessionPacket(){
        return m_psession;
    }
private:
    SessionBase *m_session;
    THandlerPacket* m_hpacket;
    PacketParser<THandlerPacket>* m_pparser;
    SessionPacket *m_psession;
};

#endif // HandlerPacketParser_HPP
