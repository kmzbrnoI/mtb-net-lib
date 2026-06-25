#include <QJsonArray>
#include <QJsonObject>
#include "led.h"
#include "../errors.h"
#include "../events.h"
#include "../main.h"

namespace MtbNetLib {

/* Daemon events ------------------------------------------------------------ */

void MtbLed::daemonGotInfo(const QJsonObject& json) {
	QString oldState = this->state;
	MtbModule::daemonGotInfo(json);
	const QJsonObject& ledJson = json[this->type_str].toObject();
	if (ledJson.contains("config"))
		this->config = ledJson["config"].toObject();
	if (oldState == "")
		this->resetOutputsState();
	if ((oldState != "active") && (oldState != "") && (this->state == "active")) {
		// Restore state of outputs
		this->restoreOutputs();
	}
	if (ledJson.contains("state")) {
		const QJsonObject& state = ledJson["state"].toObject();
		const QJsonObject& outputs = state["outputs"].toObject();
		this->inputs = state["inputs"]["packed"].toInt();
		this->outputsConfirmed = state["outputs"]["packed"].toInt();
	}

	// Must be here after setting all the attributes,
	// because hJOP can directly ask for module information
	events.call(events.onModuleChanged, this->address);

	if (MtbNetLib::state.rcs >= MtbNetLib::RcsState::stopped) {
		if ((oldState != "active") && (this->state == "active")) {
			events.call(events.onError, RCS_MODULE_RESTORED, this->address, "Module activated");
			events.call(events.onInputChanged, this->address);
			events.call(events.onOutputChanged, this->address);
		}
		if ((oldState == "active") && (this->state != "active")) {
			events.call(events.onError, RCS_MODULE_FAILED, this->address, "Module failed");
			events.call(events.onInputChanged, this->address);
			events.call(events.onOutputChanged, this->address);
		}
	}
}

void MtbLed::daemonInputsChanged(const QJsonObject& json) {
	this->inputs = json["packed"].toInt();
	events.call(events.onInputChanged, this->address);
}

void MtbLed::daemonOutputsChanged(const QJsonObject& json) {
	this->outputsConfirmed = json["packed"].toInt();
	events.call(events.onOutputChanged, this->address);
}

void MtbLed::daemonOutputsSet(const QJsonObject& json) {
	this->outputsConfirmed = json["packed"].toInt();
	events.call(events.onOutputChanged, this->address);
}

/* RCS events --------------------------------------------------------------- */

int MtbLed::rcsGetInput(unsigned int port) {
	if (port >= this->inputsCount())
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;

	return (this->inputs >> port) & 0x1;
}

int MtbLed::rcsGetOutput(unsigned int port) {
	if (port >= this->outputsCount())
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;

	return (this->outputsConfirmed >> port) & 0x1;
}

int MtbLed::rcsSetOutput(unsigned int port, int state) {
	if (port >= this->outputsCount())
		return RCS_PORT_INVALID_NUMBER;
	if (this->state != "active")
		return RCS_MODULE_FAILED;

	/* This code is intentionally commented-out!
	 * One cannot check state against confirmed state, because there can be
	 * SetOutput pending!
	 * if (this->outputsConfirmed.at(port)["value"].toInt() == state)
	 *     return 0;
	 */

	QJsonArray outputsSafe = this->config["outputsSafe"].toArray();
	if (static_cast<int>(port) >= outputsSafe.size())
		return RCS_PORT_INVALID_NUMBER;

	daemonClient.send(QJsonObject{
		{"command", "module_set_outputs"},
		{"type", "request"},
		{"address", this->address},
		{"outputs", QJsonObject{
			{QString::number(port), (state > 0)},
		}},
	});

	return 0;
}

int MtbLed::rcsGetInputType(unsigned int port) {
	return (port >= this->inputsCount()) ? RCS_PORT_INVALID_NUMBER : static_cast<int>(RcsPortInputType::iPlain);
}

int MtbLed::rcsGetOutputType(unsigned int port) {
	return (port >= this->outputsCount()) ? RCS_PORT_INVALID_NUMBER : static_cast<int>(RcsPortOutputType::oPlain);
}

void MtbLed::resetConfig() {
	this->config = {};
}

void MtbLed::resetOutputsState() {
	this->outputsConfirmed = 0;
	const QJsonArray& safeState = this->config["outputsSafe"].toArray();
	for (size_t i = 0; i < std::min<size_t>(this->outputsCount(), safeState.size()); i++)
		if (safeState[i].toBool())
			this->outputsConfirmed |= (1 << i);
}

void MtbLed::resetInputsState() {
	this->inputs = 0;
}

void MtbLed::restoreOutputs() const {
	QJsonObject outputs;
	for (size_t i = 0; i < this->outputsCount(); i++)
		outputs[QString::number(i)] = static_cast<bool>((this->outputsConfirmed >> i) & 1);

	daemonClient.send(QJsonObject{
		{"command", "module_set_outputs"},
		{"type", "request"},
		{"address", this->address},
		{"outputs", outputs},
	});
}

size_t MtbLed::inputsCount() const {
	return MtbLed::IO_CNT;
}

size_t MtbLed::outputsCount() const {
	return MtbLed::IO_CNT;
}

} // namespace MtbNetLib
