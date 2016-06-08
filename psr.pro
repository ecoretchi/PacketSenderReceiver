SUBDIRS += \
    PacketSenderReceiver.pro

DISTFILES += \
    psr.pro.shared \
    psr.pro.user

HEADERS += \
    asio_config.h \
    clientserver.hpp \
    HandlerPacket.h \
    HandlerPacketParser.hpp \
    HandlerRaw.h \
    logtrace.h \
    ngl_types.h \
    PacketParser.hpp \
    PacketParserBase.h \
    Server.hpp \
    Session.hpp \
    SessionBase.h \
    SessionPacket.h \
    tests.h \
    variableconverter.h

SOURCES += \
    main.cpp \
    test_packet.cpp \
    test_pong.cpp \
    test_raw.cpp
