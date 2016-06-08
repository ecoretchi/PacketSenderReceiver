TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

LIBS += -lpthread

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -pthread


win32 {
    DEFINES += _WIN32_WINNT=0x0501
    DEFINES += SO_UPDATE_CONNECT_CONTEXT=0x7010
    LIBS += -Lc:/MinGW/lib -lws2_32 -lmswsock

    INCLUDEPATH += e:/Libs/boost #using bind only
    INCLUDEPATH += e:/Libs/asio/include

    INCLUDEPATH += c:/MinGW/include
}

unix {
    DEFINES += ASIO_STANDALONE
    INCLUDEPATH += /home/develop/libs/asio-1.10.6/include
 }


HEADERS += \
    logtrace.h \
    variableconverter.h \
    HandlerPacketParser.hpp \
    HandlerPacket.h \
    HandlerRaw.h \
    PacketParser.hpp \
    SessionBase.h \
    Session.hpp \
    SessionPacket.h \
    PacketParserBase.h \
    Server.hpp \
    tests.h \
    asio_config.h \
    clientserver.hpp

SOURCES += \
    test_raw.cpp \
    test_packet.cpp \
    main.cpp \
    test_pong.cpp

DISTFILES += \
    ../build-PacketSenderReceiver-Desktop_ca2791-Debug/Makefile
