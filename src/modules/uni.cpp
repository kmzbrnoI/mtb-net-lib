#include <QJsonArray>
#include <QJsonObject>
#include "uni.h"

namespace MtbNetLib {

MtbUni::MtbUni(uint8_t addr) : MtbModule(addr) {}

/* Daemon events ------------------------------------------------------------ */

void MtbUni::daemonGotInfo(const QJsonObject&) {
}

void MtbUni::daemonInputsChanged(const QJsonObject&) {
}

void MtbUni::daemonOutputsChanged(const QJsonObject&) {
}

void MtbUni::daemonOutputsSet(const QJsonObject&) {
}

/* RCS events --------------------------------------------------------------- */

int MtbUni::rcsGetInput(unsigned int port) {
}

int MtbUni::rcsGetOutput(unsigned int port) {
}

int MtbUni::rcsSetOutput(unsigned int port, int state) {
}

int MtbUni::rcsGetInputType(unsigned int port) {
}

int MtbUni::rcsGetOutputsType(unsigned int port) {
}

void MtbUni::resetConfig() {
}

void MtbUni::resetState() {
}

}; // namespace MtbNetLib
