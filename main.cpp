#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <asio.hpp>
#include "logtrace.h"
#include "tests.h"
#ifdef WIN32
#define sleep Sleep
#endif
//---------------------------------------------
void test_raw()
{
    asio::thread thr(tests::raw_server);
    asio::thread thr1(tests::raw_client);
    asio::thread thr2(tests::raw_client);
}
//---------------------------------------------
void test_packet()
{
    asio::thread thr(tests::packet_server);
    asio::thread thr1(tests::packet_client);
    asio::thread thr2(tests::packet_client);
}
//---------------------------------------------
void test_pong()
{
    asio::thread thr(tests::pong_server);
    asio::thread thr1(tests::pong_client);
}//---------------------------------------------
void help()
{
    std::cout << "using args:\n";
    std::cout << "\t -test [raw|packet|pong]\t run client-server test communication\n";
    std::cout << "\t -help             \t show current help\n";
}
//---------------------------------------------
int main(int argc, char** argv)
{

    logtrace::init(new logtrace::GuardedStreamPolicy );

    std::ofstream f;
    f.open("client.log",std::ios_base::out);
    f << "Client logger v.1\n" << std::flush;
    logtrace::init(3, new logtrace::StreamPolicy(&f) );//init logger 3

    std::map<int, std::string> args;
    for(int i=0;i<argc;++i)
        args[i] = argv[i];

    if(args[1]=="-test")
    {
        if(args[2]=="raw")
            test_raw();
        else
        if(args[2]=="packet")
            test_packet();
        else
        if(args[2]=="pong")
            test_pong();
        else
            help();
    }else
    if(args[1]=="-help")
        help();

    logtrace::print() << "Press Enter to Exit" << logtrace::endl;
    std::cin.get();
    f.close();

    return 0;
}
