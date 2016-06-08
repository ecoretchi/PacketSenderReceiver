#ifndef SESSION_HPP
#define SESSION_HPP
//
// Client-server communication by using `asio` library,
// implemented send and receive functionality,
// and bring result outside of realization.
// writed by ecoretchi 02/08/2015
//
#include "asio_config.h"
#include "logtrace.h"
#include "SessionBase.h"
#include "HandlerRaw.h"
#include <asio.hpp>

//TRawHandler, first layer handle received bytes in communication
template <class TRawHandler=HandlerRaw, class TLogger=logtrace::Logger>
class Session : public SessionBase {
public:
    typedef Session<TRawHandler, TLogger> ThisSession;
public:
    Session() = delete;
    Session(asio::io_service& io_service)
        : m_socket(io_service) {
    }
    virtual ~Session(){
        loggerPrint() << "session closed" << endl;
    }
public:
    //
    // client communication socket
    //
    TSocket& socket() {
        return m_socket;
    }
    //
    // current received buffer size
    //
    virtual std::size_t streamBufferSize() const {
        return m_streamBuffer.size();
    }
public:
    virtual const TRawHandler& handler()const {
        return m_rawDataHandler;
    }
public:
    std::string handlerName() const {
        return m_rawDataHandler.name();
    }
public:
    virtual void onStarted(const asio::error_code& error) override {
        if (!error) {
            loggerPrint() << "session started" << endl ;
            m_rawDataHandler.onSessionStarted(this);
            receive();
        } else {
            loggerPrint() << "connection error: "<< error.message() << endl;
            delete this;
        }
    }
private:
    //
    // waiting any data from other side, async
    //
    virtual void receive() override {
        if(m_rawDataHandler.isSessionClosed())
        {
            delete this;
            return;
        }
#ifndef ASIO_STANDALONE
        m_socket.async_read_some(m_streamBuffer.prepare(max_rbuf),
                                boost::bind(&ThisSession::received,
                                            this,
                                            asio::placeholders::error,
                                            asio::placeholders::bytes_transferred)
                                 );
#else //c++11
        m_socket.async_read_some(m_streamBuffer.prepare(max_rbuf),
        [this](std::error_code error, std::size_t length){ this->received(error, length); });
#endif
        loggerPrint() << "waiting on receive" << endl ;
    }
private:
    //
    // callback method, occurs when some bulk of data received from othe side
    //
    void received(const asio::error_code& error, size_t bytes_transferred){
        if (!error) {
            loggerPrint() << "Received bytes: " << bytes_transferred << endl;
            m_streamBuffer.commit(bytes_transferred);
            std::istream istrm(&m_streamBuffer);
            istrm.exceptions(std::istream::failbit | std::istream::badbit);
            parseReceived(istrm);
        } else {
            loggerError() << "Receive failed: "<< error.message()<< endl;
            delete this;
        }
    }
    //
    //	part of handle_information, -> call parse policy process, and then waiting next information
    //
    virtual void parseReceived(std::istream& istrm){
        try {
            while (streamBufferSize())
                m_rawDataHandler.parsing(istrm, this); //process_information
            receive();//rady to receive next
        } catch (std::istream::failure& e) {
            if (istrm.eof()) {
                loggerPrint() << "end of data reached" << endl;
                receive();//waiting next information
            } else {
                loggerError() << "Exception at istream: " << e.what()
                              << "; possible server send unexpected information" << endl;
                delete this;
            }
        } catch (std::exception& e) {
            loggerError() << "Exception: " << e.what() << endl;
            delete this;
        }
    }
private:
    //
    //	async send string
    //
    virtual void send(const std::string& str) override{

        loggerPrint() << "sending: "<< str.size() <<" bytes" << endl;
        if(str.size()<log_max_send)
            loggerPrint() << "sending: "<< str << endl;
#ifndef ASIO_STANDALONE
        asio::async_write(m_socket,
                          asio::buffer(str),
                          asio::bind(&ThisSession::sended,
                                      this,                                      
                                      asio::placeholders::error,
                                      asio::placeholders::bytes_transferred,                                     
                                      "str"));
#else //c++11
        asio::async_write(m_socket, asio::buffer(str),
        [this](std::error_code error, std::size_t length){ this->sended(error, length, "str"); });
#endif
    }
    //
    //	async send stream buffer
    //
    virtual void send(const asio::streambuf& buf_st) override {
        asio::const_buffers_1 buf = buf_st.data();
        loggerPrint() << "sending: "<< buf_st.size() <<" bytes" << endl;
        if(buf_st.size()<log_max_send)
            loggerPrint() << "sending: "<< &buf_st << endl;

#ifdef ASIO_HAS_BOOST
        asio::async_write(m_socket,
                          buf,
                          boost_asio::bind(&ThisSession::sended,
                                      this,
                                      asio::placeholders::error,
                                      asio::placeholders::bytes_transferred,
                                      "buf"));
#else
        asio::async_write(m_socket, buf,
        [this](std::error_code error, std::size_t length){ this->sended(error, length, "buf"); });
#endif
    }
    //
    //	async send ifs data
    //
    virtual void send(int pos, int sz, std::istream& ifs) override {
        loggerPrint() << "sending from: " << pos << " pos, size: " << sz << endl;
        if (pos >= 0)
            ifs.seekg(pos);
        int sz_to = sz > max_sbuf ? max_sbuf : sz;
        size_t sz_rd = ifs.readsome(m_buff, sz_to);
        if (sz_rd > 0){
#ifdef ASIO_HAS_BOOST
            asio::async_write(m_socket, asio::buffer(m_buff, sz_rd),
                              asio::bind(&ThisSession::sended,
                                          this,
                                          asio::placeholders::error,
                                          asio::placeholders::bytes_transferred,
                                          "ifs"));
#else
        asio::async_write(m_socket, asio::buffer(m_buff, sz_rd),
        [this](std::error_code error, std::size_t length){ this->sended(error, length, "ifs");
                                                         }
        );
#endif
        } else
            loggerPrint() << "file read zero bytes" << endl;
    }
private:
    //
    // callback method, occurs when send finished
    //
    void sended(const asio::error_code& error, size_t bytes_transferred, std::string send_type){
        if (!error) {
            m_rawDataHandler.sendComplete(bytes_transferred, this, send_type);
            loggerPrint() << "send complete, bytes transfered: "<< bytes_transferred << endl;
        } else {
            m_rawDataHandler.sendCompleteWithError(error.message(), this, send_type);
            loggerPrint() << "send error: "<< error.message() << endl;
            delete this;
        }
    }
private:
    typedef typename TLogger::Stream TLStream;
    virtual TLStream& loggerPrint() {
        return TLogger::log().print()
                << socket().native_handle() << " | "
                << handlerName() << " | ";
    }
    virtual TLStream& loggerError() {
        return TLogger::log().error()
                << socket().native_handle() << " | "
                << handlerName() << " | ";
    }
    static TLStream& endl(TLStream& logstrm) {
        return TLogger::endl(logstrm);
    }
private:
    //
    // communication port
    //
    TSocket m_socket;
    //
    // transfer buffer at communication
    //
    asio::streambuf m_streamBuffer;
    //
    // handle external implementation
    //
    TRawHandler m_rawDataHandler;

    static const int max_rbuf = 1024*1024;//receive buffer 1M

    static const int max_sbuf = 1024;// send buffer while sending throgh istream
    char m_buff[max_sbuf];

    const size_t log_max_send = 30; // log data size to be loggout
};


#endif // SESSION_HPP
