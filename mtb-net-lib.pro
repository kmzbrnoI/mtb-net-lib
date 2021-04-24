TARGET = mtb-net-lib
TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle

SOURCES += \
	src/main.cpp
	src/lib-api.cpp
	src/settings.cpp

HEADERS += \
	src/main.h
	src/lib-api-common-def.h
	src/lib-api.h
	src/errors.h
	src/events.h
	src/util.h
	src/settings.h

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

VERSION_MAJOR = 0
VERSION_MINOR = 1

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR" "VERSION_MINOR=$$VERSION_MINOR"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}
DEFINES += "VERSION=\\\"$${VERSION}\\\""
