#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H
//
// Common packet communication implementation,
// using in HandlerPacketParser proxy, as part of realization
// writed by ecoretchi 02/08/2015
//
#include <ostream>
#include <string>
#include <map>
#include "variableconverter.h"
#include "logtrace.h"
#include "HandlerPacket.h"
#include "PacketParserBase.h"

template<class THandlerPacket=HandlerPacket>
class PacketParser : public PacketParserBase
{
public:
    PacketParser(THandlerPacket* handler)
        :m_handler(handler),
          packet_type("p"),
          k_type("T:"),
          k_size("S:"),
          endArg("\\"),
          endLn('n')
    {

    }
    virtual ~PacketParser(){}
public:
    typedef struct Packet{
        Packet():_dataLength(0){}
        std::string _type;
        size_t _dataLength;
        std::map<std::string, bool> _argSet;
        void clearHeader(){
            _type.clear();
            _argSet.clear();
            _dataLength=0;
        }
        std::string _data;
    } TPHeader;

public:
    virtual void constructHeader(std::ostream& ostr, const std::string& spacket) override
    {
        ostr << k_type
             << packet_type
             << endArg
             << endLn;

        ostr << k_size
             << spacket.size()
             << endArg
             << endLn;
    }
public:
    void parsing(std::istream &istrm, size_t size)
    {
        Packet& ph = m_packet;
        readArgument(istrm, ph._type, k_type);
        if(ph._type==packet_type) {
            if(readArgument(istrm, ph._dataLength, k_size))
                ph._data.clear();
            readPacketData(istrm);
        }
        else
        {
            parsing (ph._type, istrm, size);
        }
    }
public:
    virtual void parsing(const std::string& type, std::istream &/*istrm*/, size_t /*size*/)
    {
        //TODO: parsing other packet type
        throw std::string("unknow packet type received: type=" + type);
    }

protected:
    template <typename TResult>
    bool readArgument( std::istream &istrm, TResult& res, const std::string &key )
    {
        bool& ret = m_packet._argSet[key];

        if(ret)
           return true;

        std::string ln;
        ret = readArgument(istrm, ln);

        if(ret) {
            std::string val = ln.substr( ln.find(key)+key.size() );
            res = (TResult)(VarConverter(val));
            //            logtrace::print() << ((long int)this )<< "|" << __FFLINE__
            //                              << "res=" << res
            //                              << logtrace::endl;
        }

        return ret;
    }
protected:
    void readPacketData( std::istream &istrm )
    {
        std::string& res = m_packet._data;
        size_t size = m_packet._dataLength;

        //        logtrace::print() << ((long int)this ) << "|" << __FFLINE__
        //                          << "_dataLength=" << size << logtrace::endl;

        if(!size)
            return;

        unsigned int sz = 1024;
        char buff[sz];
        res.reserve(size);
        do{
            if(size<sz)
                sz = size;
            int lenread = istrm.readsome(buff,sz);
            buff[lenread] = 0;

            //            logtrace::print() << ((long int)this )<< "|" << __FFLINE__
            //                              << "lenread=" << lenread
            //                              << "; buff=" << buff
            //                              << logtrace::endl;

            if(!lenread)
                break;
            res.append(buff, lenread);
            size-=lenread;

        }while(size);

        if(res.size() == m_packet._dataLength){
            m_handler->onPacketReceived(res);
            m_packet.clearHeader();
        }

    }
protected:
    bool readArgument(std::istream& istrm, std::string& ln)
    {        
        std::getline(istrm, ln, endLn);
        if(m_portionArg.size())
            ln = m_portionArg+ln;
        if(ln.find(endArg) != ln.size()-1){
            m_portionArg=ln; // argument not completed was read
            return false;
        }
        m_portionArg.clear();
        return true;
    }
protected:
    Packet m_packet;
    std::string m_portionArg;
    THandlerPacket* m_handler;
protected:
    std::string packet_type;
    std::string k_type;
    std::string k_size;
    std::string endArg;
    char endLn;
};

#endif // PACKETHANDLER_H
