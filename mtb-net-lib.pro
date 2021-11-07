TARGET = mtb-net-lib
TEMPLATE = lib

SOURCES += \
	src/main.cpp \
	src/settings.cpp \
	src/events.cpp \
	src/client.cpp \
	src/modules/uni.cpp \
	src/lib-api.cpp

HEADERS += \
	src/main.h \
	src/lib-api-common-def.h \
	src/lib-api.h \
	src/errors.h \
	src/events.h \
	src/util.h \
	src/settings.h \
	src/client.h \
	src/modules/module.h \
	src/modules/uni.h

CONFIG += c++17 dll
QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -std=c++17

win32 {
	QMAKE_LFLAGS += -Wl,--kill-at
	QMAKE_CXXFLAGS += -enable-stdcall-fixup
}
win64 {
	QMAKE_LFLAGS += -Wl,--kill-at
	QMAKE_CXXFLAGS += -enable-stdcall-fixup
}

QT -= gui
QT += core serialport network

VERSION_MAJOR = 1
VERSION_MINOR = 2

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR" "VERSION_MINOR=$$VERSION_MINOR"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}
DEFINES += "VERSION=\\\"$${VERSION}\\\""
