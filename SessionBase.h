#ifndef SESSIONBASE_HPP
#define SESSIONBASE_HPP
//
// Basic session interface
// writed by ecoretchi 02/08/2015
//
#include <string>
#include <asio.hpp>

class SessionBase
{
public:
    typedef asio::ip::tcp::tcp::socket TSocket;
public:
    virtual ~SessionBase(){}
public:
    virtual void onStarted(const asio::error_code& ) = 0;
    virtual void receive() = 0;
    virtual void send(const std::string&) = 0;
    virtual void send(const asio::streambuf&) = 0;
    virtual void send(int /*pos*/, int /*sz*/, std::istream& /*ifs*/) =0;
    virtual std::size_t streamBufferSize() const =0;
public:
    virtual TSocket& socket() = 0;
};
#endif // SESSIONBASE_HPP
