//
// Test client-server communication protocol send/receive
// transported throw packet data,
// writed by ecoretchi 02/08/2015
//
#include "tests.h"
#include "HandlerPacketParser.hpp"

using asio::ip::tcp;
namespace  tests {
namespace packeted {
//---------------------------------------------
class HandlerPacketClient : public HandlerPacket
{
public:
    virtual void onSessionStarted(SessionPacket *session) override {
        for(int i=0;i<4;++i){
            std::stringstream ss;
            ss <<i<< "{fromClient}";
            session->sendPacket(ss.str());
        }
    }
    virtual void onPacketReceived(const std::string &packet) override {
        logtrace::Logger3::log().print()
                << "packet received -> \n\t\t\t"
                << packet << logtrace::endl;
    }
};
//---------------------------------------------
class TestPacketHandlerClient: public HandlerPacketParser<HandlerPacketClient>
{
public:
    virtual std::string name() const override{ return "TestPacketHandlerClient"; }
};
//---------------------------------------------
class HandlerPacketServer: public HandlerPacket
{
public:
    virtual void onSessionStarted(SessionPacket *session) override {
        for(int i=0;i<1;++i){
            std::stringstream ss;
            ss <<i<< "{fromServer}";
            session->sendPacket(ss.str());
        }
    }
    virtual void onPacketReceived(const std::string &packet) override {
        logtrace::print()
                << "packet received -> \n\t\t\t"
                << packet << logtrace::endl;
    }
};
//---------------------------------------------
class TestPacketHandlerServer: public HandlerPacketParser<HandlerPacketServer>
{
public:
    virtual std::string name() const { return "TestPacketHandlerServer"; }
};

}//end namespace packeted

void packet_client(){ client<packeted::TestPacketHandlerClient,logtrace::Logger3 >(); }
void packet_server(){ server<packeted::TestPacketHandlerServer>(); }

}//end namespace tests
