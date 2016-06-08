#ifndef RawHandler_H
#define RawHandler_H
//
// Handler of client-server communication
// writed by ecoretchi 02/08/2015
//
#include <istream>
#include <string>
class SessionBase;
class HandlerRaw
{
public:
    virtual std::string name() const { return "RawHandler"; }
public:
    virtual void onSessionStarted(SessionBase*) {}
    virtual bool isSessionClosed() { return false; }
public:
    virtual void sendComplete(size_t, SessionBase*, std::string ) {}
    virtual void sendCompleteWithError(std::string, SessionBase*, std::string) {}
public:
    virtual void parsing(std::istream& /*receivedData*/, SessionBase*) {}

};

#endif // RawHandler_H
