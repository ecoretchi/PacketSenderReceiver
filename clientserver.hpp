#ifndef CLIENTSERVER_HPP
#define CLIENTSERVER_HPP

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <assert.h>
#include "variants.h"
#include "logtrace.h"
#include "asio_config.h"
#include "Session.hpp"
#include "Server.hpp"

typedef std::shared_ptr<asio::io_service> asioios_shared;
//blocking function, without any processing for exceptions
template <class TSession, class TLogger=logtrace::Logger>
void run_client(asioios_shared io_service, int port, const std::string& host = "localhost"){
    if(!io_service)
        io_service = std::make_shared<asio::io_service>();

    asio::ip::tcp::tcp::resolver resolver(io_service);
    asio::ip::tcp::tcp::resolver::query query(asio::ip::tcp::tcp::v4(), host, variants::to_string(port) );
    asio::ip::tcp::tcp::resolver::iterator endpoint_iter = resolver.resolve(query);

    SessionBase* client_session = new Session<TSession, TLogger>(*io_service);
#ifndef ASIO_STANDALONE
    asio::async_connect(client_session->socket(),
                        endpoint_iter,
                        boost::bind(&SessionBase::onStarted,
                                    client_session,
                                    asio::placeholders::error)
                        );
#else //c++11
    asio::async_connect(client_session->socket(), endpoint_iter,
        [client_session](asio::error_code err, asio::ip::tcp::tcp::resolver::iterator){
            client_session->onStarted(err);
    });
#endif
    io_service->run();
}
//blocked function, but catch all exceptions
template <class TSession, class TLogger=logtrace::Logger>
void new_client(asioios_shared io_service, int port=1234, const std::string& host = "localhost"){
    std::cout << "client thread here\n";
    try {
        run_client<TSession, TLogger>(io_service, port, host);
    }
    catch(std::exception& e){
        std::cout<< e.what() << "\n";
    }
    catch(...)
    {
        std::cout<< "client exception\n";
    }
}
//blocking function, without any processing for exceptions
template <class TSession, class TLogger=logtrace::Logger >
void run_server(asioios_shared io_service, int port){
    if(!io_service)
        io_service = std::make_shared<asio::io_service>();
    Server< Session<TSession,TLogger> > srv(*io_service,port);
    io_service->run();
}
//blocked function, but catch all exceptions
template <class TSession, class TLogger=logtrace::Logger >
void new_server(asioios_shared io_service, int port){
    std::cout << "server thread here\n";
    try {
        run_server<TSession, TLogger>(io_service, port);
    }
    catch(std::exception& e){
        std::cout<< e.what() << "\n";
    }
    catch(...)
    {
        std::cout<< "server exception\n";
    }
}

#endif // CLIENTSERVER_HPP

