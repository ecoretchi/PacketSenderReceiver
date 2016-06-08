#ifndef TESTS_H
#define TESTS_H

#include "clientserver.hpp"

namespace tests { //call as thread callback

void raw_client();
void raw_server();

void packet_client();
void packet_server();

void pong_client();
void pong_server();

template <class TSession, class TLogger=logtrace::Logger>
void client(){
    new_client<TSession, TLogger>();
}

template <class TSession, class TLogger=logtrace::Logger >
void server(){
    new_server<TSession, TLogger>(1234);
}

}

#endif // TESTS_H
