//
// Test client-server communication protocol send/receive
// transported through raw-data,
// writed by ecoretchi 02/08/2015
//

#include "tests.h"

using asio::ip::tcp;
namespace tests{
namespace raw {
const int testRawSize = 1245;
//---------------------------------------------
class TestRawHandlerClient : public HandlerRaw
{
public:
    virtual std::string name() const {
        return "TestRawHandlerClient";
    }
    virtual void onSessionStarted(SessionBase *session){
            asio::streambuf buf;
            std::ostream os(&buf);//using ostream with asio streambuf, to prepare raw data to send
            for(int i=1;i<=testRawSize;++i)
                os << i%10;
            session->send(buf);
            session->send("1;2;3;4;5");// send to check command blocks read on raw server
    }
};
//---------------------------------------------
class TestRawHandlerServer : public HandlerRaw
{
    int m_pos;//remember position of begining check
public:
    TestRawHandlerServer ():m_pos(0){}
public:
    virtual std::string name() const {
        return "TestRawHandlerServer";
    }
    virtual void parsing(std::istream &istrm, SessionBase */*session*/) {

        int part_sz = 50;
        char buf[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
        std::stringstream ss;
        for(int i=1;i<=testRawSize;++i)
            ss<<i%10;
        std::string expected = ss.str();
        while(m_pos<testRawSize){
            if(part_sz>(testRawSize-m_pos))
                part_sz = testRawSize-m_pos;
            int sz = istrm.readsome(buf,part_sz);
            if(!sz){
                assert(part_sz<testRawSize);//but somthings should be read
                return;//nothings to read
            }
            buf[sz] = 0;
            logtrace::print() << "parsed RAW ->\n\t\t\t" << buf << logtrace::endl;
            for(int i=m_pos;i<sz;++i){

                //logtrace::print() << (i+m_pos) << "#" << expected[idx] << logtrace::endl;
                assert(buf[i]==expected[i+m_pos]);
            }
            part_sz-=sz;
            m_pos+=sz;
        }
        if(m_pos==testRawSize){
            std::string ln;
            const char* expectedCmds = "12345";
            //check other type to read, line comma separated
            for(int i=0;i<5;++i){
                std::getline(istrm, ln, ';');
                assert(ln[0]==expectedCmds[i]);
                logtrace::print() << "parsed LN ->\n\t\t\t" << ln << logtrace::endl;
            }
        }
    }
};

} // namespace raw

//---------------------------------------------
void raw_client(){ client<raw::TestRawHandlerClient, logtrace::Logger3 >(); }
void raw_server(){ server<raw::TestRawHandlerServer>(); }
//---------------------------------------------

} // namespace tests
