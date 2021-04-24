#include <QJsonArray>
#include "main.h"
#include "events.h"
#include "modules/uni.h"
#include "errors.h"

namespace MtbNetLib {

AppThread main_thread;
MtbUsb mtbusb;
std::array<std::unique_ptr<MtbModule>, MAX_MODULES> modules;
DaemonClient daemonClient;
Settings settings;
State state;
LibMain libMain;

void log(const QString& message, LogLevel loglevel) {
	if (loglevel <= state.loglevel)
		events.call(events.onLog, static_cast<int>(loglevel), message);
}

LibMain::LibMain() {
	QObject::connect(&daemonClient, SIGNAL(onConnected()), this, SLOT(daemonConnected()));
	QObject::connect(&daemonClient, SIGNAL(onDisconnected()), this, SLOT(daemonDisconnected()));
	QObject::connect(&daemonClient, SIGNAL(jsonReceived(const QJsonObject&)),
	                 this, SLOT(daemonReceived(const QJsonObject&)));
}

void LibMain::daemonConnected() {
	log("Connected to daemon server.", LogLevel::Info);

	// TODO: add timeout, report error on timeout

	QJsonArray allModules;
	for (size_t i = 1; i < MAX_MODULES; i++)
		allModules.push_back(static_cast<int>(i));

	daemonClient.send(QJsonObject{
		{"command", "module_subscribe"},
		{"type", "request"},
		{"addresses", allModules},
	});
	daemonClient.send(QJsonObject{
		{"command", "mtbusb"},
		{"type", "request"},
	});
}

void LibMain::daemonDisconnected() {
	log("Disconnected from daemon server", LogLevel::Info);

	if (state.rcs == RcsState::opening) {
		events.call(events.onError, RCS_NOT_OPENED, 0,
					"Unable to connect to MTB daemon server");
	}

	state.rcs = RcsState::closed;
	mtbusb.connected = false;
	for (size_t i = 0; i < MAX_MODULES; i++) {
		if (modules[i] != nullptr) {
			modules[i]->resetState();
			modules[i]->resetConfig();
		}
	}

	events.call(events.afterClose);
}

void LibMain::daemonReceived(const QJsonObject& json) {
	const QString command = json["command"].toString();
	const QString type = json["type"].toString();

	if ((json["type"].toString() == "response") && (json["status"].toString() != "ok")) {
		const QJsonObject& error = json["error"].toObject();
		log("Got error response for command '"+command+"': "+error["message"].toString(),
		    LogLevel::Warning);
		return;
	}

	if (command == "mtbusb") {
		this->daemonReceivedMtbUsb(json);

	} else if (command == "module") {
		size_t addr = json["module"].toObject()["address"].toInt();
		const QString& type = json["module"].toObject()["type"].toString();
		if (modules[addr] == nullptr) {
			if (type.startsWith("MTB-UNI"))
				modules[addr] = std::make_unique<MtbUni>(json);
			else
				modules[addr] = std::make_unique<MtbModule>(json);
		} else {
			modules[addr]->daemonGotInfo(json);
		}

	} else if (command == "modules") {
		const QJsonObject& jsonModules = json["modules"].toObject();
		for (size_t i = 0; i < MAX_MODULES; i++) {
			if (jsonModules.contains(QString::number(i))) {
				const QJsonObject& jsonModule = jsonModules[QString::number(i)].toObject();
				if (modules[i] == nullptr) {
					const QString& type = jsonModule["type"].toString();
					if (type.startsWith("MTB-UNI"))
						modules[i] = std::make_unique<MtbUni>(jsonModule);
					else
						modules[i] = std::make_unique<MtbModule>(jsonModule);
				} else {
					modules[i]->daemonGotInfo(jsonModule);
				}
			} else  if (modules[i] != nullptr) {
				modules[i] = nullptr; // TODO: send module failed event?
			}
		}

		if (state.rcs == RcsState::opening) {
			log("Got modules", LogLevel::Info);
			events.call(events.afterOpen);
		}

	} else if (command == "module_inputs_changed") {
		const QJsonObject& moduleInputsChanged = json["module_inputs_changed"].toObject();
		size_t addr = moduleInputsChanged["address"].toInt();
		if (modules[addr] != nullptr)
			modules[addr]->daemonInputsChanged(moduleInputsChanged);

	} else if (command == "module_outputs_changed") {
		const QJsonObject& moduleOutputsChanged = json["module_outputs_changed"].toObject();
		size_t addr = moduleOutputsChanged["address"].toInt();
		if (modules[addr] != nullptr)
			modules[addr]->daemonOutputsChanged(moduleOutputsChanged);

	} else if (command == "module_set_outputs") {
		const QJsonObject& outputs = json["outputs"].toObject();
		size_t addr = json["address"].toInt();
		if (modules[addr] != nullptr)
			modules[addr]->daemonOutputsSet(outputs);

	}
}

void LibMain::daemonReceivedMtbUsb(const QJsonObject& json) {
	const QJsonObject jsonMtbUsb = json["mtbusb"].toObject();
	MtbUsb old = mtbusb;

	mtbusb.connected = jsonMtbUsb["connected"].toBool();
	mtbusb.type = jsonMtbUsb["type"].toInt();
	mtbusb.speed = jsonMtbUsb["speed"].toInt();
	mtbusb.fw_version = jsonMtbUsb["firmware_version"].toString();
	mtbusb.proto_version = jsonMtbUsb["protocol_version"].toString();

	if ((old.connected) && (!mtbusb.connected) && (state.rcs == RcsState::started)) {
		log("Disconnected from MTB-USB!", LogLevel::Error);
		state.rcs = RcsState::stopped;
		events.call(events.afterStop);
		for (size_t i = 0; i < MAX_MODULES; i++)
			if (modules[i] != nullptr)
				modules[i]->resetState();
	}
	if ((!old.connected) && (mtbusb.connected)) {
		log("Connected to MTB-USB", LogLevel::Info);
		if (state.rcs == RcsState::opening) {
			// If not opening, module state is received via module event
			log("Getting modules info...", LogLevel::Info);
			daemonClient.send(QJsonObject{
				{"command", "modules"},
				{"type", "request"},
				{"satte", true},
			});
		}
	}
}

} // MtbNetLib
