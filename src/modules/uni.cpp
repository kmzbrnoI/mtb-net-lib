#include <QJsonArray>
#include <QJsonObject>
#include "uni.h"
#include "../errors.h"

namespace MtbNetLib {

MtbUni::MtbUni(const QJsonObject& json) : MtbModule(json) {}

/* Daemon events ------------------------------------------------------------ */

void MtbUni::daemonGotInfo(const QJsonObject& json) {
	MtbModule::daemonGotInfo(json);
	const QJsonObject& uniJson = json[this->type_str].toObject();
	this->ir = uniJson["ir"].toBool();
	if (uniJson.contains("config"))
		this->config = uniJson["config"].toObject();
	if (uniJson.contains("state")) {
		const QJsonObject& state = uniJson["state"].toObject();
		const QJsonObject& outputs = state["outputs"].toObject();
		for (const auto& key : outputs.keys())
			this->outputsConfirmed[key.toInt()] = outputs[key].toObject();
		this->inputs = state["inputsPacked"].toInt();
	}
}

void MtbUni::daemonInputsChanged(const QJsonObject& json) {
}

void MtbUni::daemonOutputsChanged(const QJsonObject& json) {
}

void MtbUni::daemonOutputsSet(const QJsonObject& json) {
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
