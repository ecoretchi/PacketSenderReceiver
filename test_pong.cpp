//
// Test client-server communication protocol send/receive
// transported pong packet data
// from client to server and server pong to client, and so on loops forever
// writed by ecoretchi 02/08/2015
//
#include "tests.h"
#include "HandlerPacketParser.hpp"

using asio::ip::tcp;
namespace tests{
namespace pong {
class TestHandlerClient;
//---------------------------------------------
class HandlerPongCS: public HandlerPacket
{
public:
    HandlerPongCS():m_session(0){}

public:
    virtual void onSessionStarted(SessionPacket *session) override {
        m_session = session;
    }
    virtual void onPacketReceived(const std::string &packet) override {
        logtrace::print()
                << "packet received -> \n\t\t\t"
                << packet << logtrace::endl;
        std::size_t val = VarConverter(packet);
        val++;
        if(m_session){
            m_session->sendPacket(std::to_string(val));
        }
    }
private:
    SessionPacket *m_session;
};
//---------------------------------------------
class TestHandlerClient: public HandlerPacketParser<HandlerPongCS>
{
public:
    virtual std::string name() const override{
        return "TestClientPong";
    }
    virtual void onSessionStarted(SessionBase *session) override {
        //extend(at low layer) start client session
        //origin call
        HandlerPacketParser<HandlerPongCS>::onSessionStarted(session);
        //init first pong
        this->sessionPacket()->sendPacket(std::to_string(0));
    }
};
//---------------------------------------------
class TestHandlerServer: public HandlerPacketParser<HandlerPongCS>
{
public:
    virtual std::string name() const { return "TestServerPong"; }
};
} // namespace pong

void pong_client(){ client<pong::TestHandlerClient,logtrace::Logger3 >(); }
void pong_server(){ server<pong::TestHandlerServer>(); }

} // namespace tests
