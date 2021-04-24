#include <QJsonArray>
#include <QJsonObject>
#include "uni.h"
#include "../errors.h"

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
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;
}

int MtbUni::rcsGetOutput(unsigned int port) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;
}

int MtbUni::rcsSetOutput(unsigned int port, int state) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;
}

int MtbUni::rcsGetInputType(unsigned int port) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
}

int MtbUni::rcsGetOutputType(unsigned int port) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
}

void MtbUni::resetConfig() {
}

void MtbUni::resetState() {
}

size_t MtbUni::inputsCount() const { return UNI_IO_CNT; }
size_t MtbUni::outputsCount() const { return UNI_IO_CNT; }

}; // namespace MtbNetLib
