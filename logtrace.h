#ifndef LOGTRACE_H
#define LOGTRACE_H
//
// Make logs utility
// writed by ecoretchi 02/08/2015
//

#include <iostream>
#include <exception>
#include <iomanip>
#include <map>
#include <assert.h>
#include <asio/detail/mutex.hpp>

#define __FFLINE__ __FILE__<<":"<< __LINE__ <<"::"<<__FUNCTION__ << "(); "

namespace logtrace {

//log stream interface
struct Logstream {
    Logstream():_ownerId(0){}
    virtual ~Logstream(){}
    virtual bool guarded() { return false; }
    virtual Logstream& operator << ( Logstream&(*)(Logstream&) ) = 0;
    virtual Logstream& operator << (long int)=0;
    virtual Logstream& operator << (const char*)=0;
    virtual Logstream& operator << (short int)=0;
    virtual Logstream& operator << (int)=0;
    virtual Logstream& operator << (unsigned int)=0;
    virtual Logstream& operator << (char)=0;
    virtual Logstream& operator << (const std::string&)=0;
    virtual Logstream& operator << (const std::streambuf*)=0;
    virtual Logstream& operator << (std::ostream*)=0;
    virtual Logstream& otream() { return *this; }
    void setOwnerId(int id) { _ownerId = id; }
    int ownerId() const { return _ownerId; }
private:
    int _ownerId;
};
//empty streamer
struct EmptyPolicy : public Logstream {
    virtual ~EmptyPolicy(){}    
    virtual Logstream& operator << ( Logstream&(*)(Logstream&) ){return otream(); }
    virtual Logstream& operator << (long int){return otream();}
    virtual Logstream& operator << (const char*){return otream();}
    virtual Logstream& operator << (short int){return otream();}
    virtual Logstream& operator << (int){return otream();}
    virtual Logstream& operator << (unsigned int){return otream();}
    virtual Logstream& operator << (char){return otream();}
    virtual Logstream& operator << (const std::string&){return otream();}
    virtual Logstream& operator << (const std::streambuf*){return otream();}
    virtual Logstream& operator << (std::ostream*){return otream();}

};
//thread unsafe output streamer
struct StreamPolicy : public Logstream {
    typedef enum EDoFlush {
        None,
        OnAllOperators,
        OnFnOperator
    } DoFlush;

    StreamPolicy ():_ostr(&std::cout),_doflush(None) { }
    StreamPolicy (std::ostream* ostr, DoFlush flag = OnFnOperator):_ostr(ostr),_doflush(flag) { }

    virtual ~StreamPolicy(){}
    virtual Logstream& operator << ( Logstream&(*fn)(Logstream&) ){
        (*fn)(*this);
        if(_doflush==OnFnOperator)
            _ostr->flush();
        return otream();
    }
    virtual Logstream& operator << (long int val){*_ostr << val;return otream();}
    virtual Logstream& operator << (const char* val){*_ostr << val;return otream();}
    virtual Logstream& operator << (short int val){*_ostr << val;return otream();}
    virtual Logstream& operator << (int val){*_ostr << val;return otream();}
    virtual Logstream& operator << (unsigned int val){*_ostr << val;return otream();}
    virtual Logstream& operator << (char val){*_ostr << val;return otream();}
    virtual Logstream& operator << (const std::string& val){*_ostr << val;return otream();}
    virtual Logstream& operator << (const std::streambuf* val){*_ostr << (std::streambuf*)val;return otream();}
    virtual Logstream& operator << (std::ostream* val){_ostr=val; return otream();}
    virtual Logstream& otream() {
        if(_doflush==OnAllOperators)
            _ostr->flush();
        return *this;
    }
    std::ostream* _ostr;
    DoFlush _doflush;
};

//thread safe output streamer
struct GuardedStreamPolicy : public StreamPolicy {
    GuardedStreamPolicy(){}
    GuardedStreamPolicy (std::ostream* ostr):StreamPolicy(ostr) { }
    virtual bool guarded() { return true; }
};

class LoggerException : public std::exception {
public:
    LoggerException(const std::string& s):_message(s){}
    virtual const char* what() const _GLIBCXX_USE_NOEXCEPT{
        return _message.c_str();
    }
private:
    std::string _message;
};

//Logger specialist
class LogHandl{
public:
    LogHandl() = delete;
    LogHandl(Logstream* logPolicy, int logNum)
        : m_stream(logPolicy),
          m_logNum(logNum),
          m_lockCount(0){
        if(!logPolicy)
            throw LoggerException("log policy not set");
        m_guarded = logPolicy->guarded();
        logPolicy->setOwnerId(logNum);
        stream() << "LogHandl v.01; logNum=" << logNum << "\n";
    }
    ~LogHandl(){ delete m_stream; }
public:
    //thread unsafe logstream
    Logstream& stream(){
        return *m_stream;
    }
public:
    //thread safe logstreams ( if guard policy true )
    //WARNIG: if using guarded log policy, be shure using methods
    //        print(), info(), error() or warning() with endl() method ligament,
    //        for example: print() << "print somthing" << endl;
    Logstream& print(){
        lock();
        return stream();
    }
    Logstream& info(){
        lock();
        return stream() << "INFO: ";
    }
    Logstream& error(){
        lock();
        return stream() << "ERROR: ";
    }
    Logstream& warning(){
        lock();
        return stream() << "WARNING: ";
    }
    Logstream& endl() {
        stream() << "\n";
        unlock();
        return stream();
    }
private:
    void guardLogger(bool lock){
        if(!m_guarded)
            return;
///        debug information
//        stream() << "locker count = " << m_lockCount << "\n";
        if(lock)
            m_mutex.lock();
        else
            m_mutex.unlock();
    }
    void lock() {
        m_lockCount++;
        guardLogger(true);
    }
    void unlock() {
        m_lockCount--;
        guardLogger(false);
    }

private:
    Logstream* m_stream;
    int m_logNum;
    bool m_guarded;
    asio::detail::mutex m_mutex;
    int m_lockCount;
};

class LogsManager{
    LogsManager(){ }
protected:
    static LogsManager& inst(){
        static LogsManager* logmng = 0;
        if(!logmng)
            logmng = new LogsManager();
        return *logmng;
    }
public:
    // return null if log not exist by logid
    static LogHandl* log(int logid){
        std::map<int, LogHandl*>::iterator itr = inst().m_logs.find(logid);
        if(itr != inst().m_logs.end())
            return itr->second;
        return 0;
    }
    // create new log by logid and it policy, but if
    // already was created return log handle on previously created
    static LogHandl* newLog(int logid, Logstream* logPolicy){
        LogHandl*  logh = inst().m_logs[logid];
        if(!logh){
            logh = new LogHandl(logPolicy, logid);
            inst().m_logs[logid] = logh;
        }
        return logh;
    }
    static int size() {
        return inst().m_logs.size();
    }
private:
    std::map<int, LogHandl*> m_logs;
};

template <int I>
class BasicLogger
{
public:
    typedef Logstream Stream;
    typedef Logstream&(*logstreamFn)(Logstream&);
public:
    static LogHandl& log(Logstream* logPolicy=0){
        static LogHandl* stLog(0);
        if(!stLog){
            stLog = LogsManager::newLog(I, logPolicy);
        }
        return *stLog;
    }
    static Logstream& endl(Logstream&) {
        return log().endl();
    }
};

//example for construct some loggers, alternative as LogsManager
typedef BasicLogger<1> Logger;
typedef BasicLogger<1> Logger1;//same as Logger
typedef BasicLogger<2> Logger2;
typedef BasicLogger<3> Logger3;
typedef BasicLogger<4> Logger4;
typedef BasicLogger<5> Logger5;

//alias function
inline void init(Logstream* logPolicy) {
    Logger::log(logPolicy);
}
inline void init(int logId, Logstream* logPolicy) {
    LogsManager::newLog(logId,logPolicy);
}
//alias stream Logger
inline Logstream& print(){
    return Logger::log().print();
}
//alias stream object from Logger  - INFO
inline Logstream& info(){
    return Logger::log().info();
}
//alias stream object from Logger  - ERROR
inline Logstream& error(){
    return Logger::log().error();
}
//alias stream object from Logger - WARNING
inline Logstream& warning(){
    return Logger::log().warning();
}
inline Logstream& endl(Logstream& strm) {
    LogHandl* lh = LogsManager::log( strm.ownerId() );
    return lh?lh->endl():strm;
}
}//end namespace logtrace
#endif // LOGTRACE_H
