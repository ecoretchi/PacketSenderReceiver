#ifndef SERVER_HPP
#define SERVER_HPP
//
// Basic communication server layer
// writed by ecoretchi 02/08/2015
//
#include "asio_config.h"
#include "logtrace.h"

#include <SessionBase.h>

// TSession is estabilished connection with client
template <class TSession/*=SessionBase*/, class TLogger=logtrace::Logger >
class Server {
public:
    Server(asio::io_service& io_service, unsigned short port)
        :_io_service(io_service),
          _acceptor(io_service,
                    asio::ip::tcp::tcp::endpoint(
                        asio::ip::tcp::tcp::v4(),
                        port))
    {
        startAccept();
        loggerPrint() << "Server started, listen on port: "<< port << endl;
    }
    virtual ~Server(){
        loggerPrint() << "Server closed" << endl;
    }

private:
    void startAccept()
    {
        SessionBase* session = new TSession(_io_service);//async session object
#ifndef ASIO_STANDALONE
        _acceptor.async_accept(session->socket(),
                               boost::bind(&Server::handleAccept, this, session,
                                           asio::placeholders::error));
#else //c++11
        _acceptor.async_accept(session->socket(),
            [this, session](asio::error_code err){ this->handleAccept(session,err); });
#endif
    }
    void handleAccept(SessionBase* session, const asio::error_code& error)
    {
        session->onStarted(error);
        startAccept(); // waiting other user connection
    }
private:
    typedef typename TLogger::Stream TLStream;
    virtual TLStream& loggerPrint() {
        return TLogger::log().print();
    }
    virtual TLStream& loggerError() {
        return TLogger::log().error();
    }
    static TLStream& endl(TLStream& logstrm) {
        return TLogger::endl(logstrm);
    }
private:
    asio::io_service& _io_service;
    asio::ip::tcp::acceptor _acceptor;
};
#endif // SERVER_HPP
