#include <QJsonArray>
#include <QJsonObject>
#include "uni.h"
#include "../errors.h"
#include "../events.h"
#include "../main.h"

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
	this->inputs = json["packed"].toInt();
	events.call(events.onInputChanged, this->address);
}

void MtbUni::daemonOutputsChanged(const QJsonObject& json) {
	for (const auto& key : json.keys())
		this->outputsConfirmed[key.toInt()] = json[key].toObject();
	events.call(events.onOutputChanged, this->address);
}

void MtbUni::daemonOutputsSet(const QJsonObject& json) {
	for (const auto& key : json.keys())
		this->outputsConfirmed[key.toInt()] = json[key].toObject();
}

/* RCS events --------------------------------------------------------------- */

int MtbUni::rcsGetInput(unsigned int port) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;

	return (this->inputs >> port) & 0x1;
}

int MtbUni::rcsGetOutput(unsigned int port) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;

	return this->outputsConfirmed[port]["value"].toInt();
}

int MtbUni::rcsSetOutput(unsigned int port, int state) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;

	QString portType = this->config["outputsSafe"].toArray()[port].toObject()["type"].toString();
	if ((state > 1) && (portType == "plain"))
		portType = "flicker";

	daemonClient.send(QJsonObject{
		{"command", "module_set_outputs"},
		{"type", "request"},
		{"address", this->address},
		{"outputs", QJsonObject{
			{QString::number(port), QJsonObject{
				{"type", portType},
				{"value", state},
			}},
		}},
	});

	return 0;
}

int MtbUni::rcsGetInputType(unsigned int port) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;

	if (this->config.contains("irsPacked")) {
		uint16_t irs = this->config["irsPacked"].toInt();
		irs >>= port;
		if (irs & 1)
			return static_cast<int>(RcsPortInputType::iIr);
	}
	return static_cast<int>(RcsPortInputType::iPlain);
}

int MtbUni::rcsGetOutputType(unsigned int port) {
	if (port > UNI_IO_CNT)
		return RCS_PORT_INVALID_NUMBER;

	const QString& portType = this->config["outputsSafe"].toArray()[port].toObject()["type"].toString();
	if (portType == "s-com")
		return static_cast<int>(RcsPortOutputType::oScom);
	return static_cast<int>(RcsPortOutputType::oPlain);
}

void MtbUni::resetConfig() {
	this->config = {};
}

void MtbUni::resetState() {
	this->inputs = 0;
	for (size_t i = 0; i < UNI_IO_CNT; i++)
		this->outputsConfirmed[i] = {};
}

size_t MtbUni::inputsCount() const { return UNI_IO_CNT; }
size_t MtbUni::outputsCount() const { return UNI_IO_CNT; }

} // namespace MtbNetLib