#include <QHostAddress>
#include <QFile>
#include "lib-api.h"
#include "errors.h"
#include "main.h"
#include "util.h"

/* This file deafines all library exported API functions. */

namespace MtbNetLib {

unsigned int rcs_api_version = 0;

///////////////////////////////////////////////////////////////////////////////
// Open/close

int Open() {
	if (state.rcs != RcsState::closed)
		return RCS_ALREADY_OPENNED;

	try {
		events.call(events.beforeOpen);

		state.rcs = RcsState::opening;
		daemonClient.connect(
			QHostAddress(settings["server"]["host"].toString()),
			settings["server"]["port"].toInt(),
			settings["server"]["keepAlive"].toBool()
		);
		// Potential error is reporter later
		return 0;
	} catch (...) {
		state.rcs = RcsState::closed;
		return RCS_GENERAL_EXCEPTION;
	}
}

int Close() {
	if (state.rcs == RcsState::closed)
		return RCS_NOT_OPENED;

	try {
		events.call(events.beforeClose);
		daemonClient.disconnect();
		return 0;
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

bool Opened() {
	return state.rcs >= RcsState::stopped;
}

///////////////////////////////////////////////////////////////////////////////
// Start/stop

int Start() {
	if (state.rcs == RcsState::started)
		return RCS_ALREADY_STARTED;
	if (state.rcs != RcsState::stopped)
		return RCS_NOT_OPENED;
	if (!mtbusb.connected) {
		log("Unable to start communication, MTB daemon disconnected from MTB-USB", LogLevel::Error);
		return RCS_NOT_OPENED;
	}

	try {
		events.call(events.beforeStart);
		state.rcs = RcsState::started;
		events.call(events.afterStart);
		events.call(events.onScanned);
		return 0;
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

int Stop() {
	if (state.rcs != RcsState::started)
		return RCS_NOT_STARTED;

	try {
		events.call(events.beforeStop);
		state.rcs = RcsState::stopped;
		for (size_t i = 0; i < MAX_MODULES; i++)
			if (modules[i] != nullptr)
				modules[i]->resetOutputsState();
		daemonClient.send(QJsonObject{
			{"command", "reset_my_outputs"},
			{"type", "request"},
		});
		return 0;
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

bool Started() {
	return state.rcs == RcsState::started;
}

///////////////////////////////////////////////////////////////////////////////
// Config

int LoadConfig(char16_t *filename) {
	if (daemonClient.connected())
		return RCS_FILE_DEVICE_OPENED;

	try {
		const QString& qfilename = QString::fromUtf16(filename);
		if (QFile::exists(qfilename)) {
			settings.load(qfilename);
		} else {
			log("Configuration file does not exist, creating...", LogLevel::Info);
			settings.save(qfilename); // save default config
		}
		return 0;
	} catch (...) { return RCS_FILE_CANNOT_ACCESS; }

}

int SaveConfig(char16_t *filename) {
	try {
		settings.save(QString::fromUtf16(filename));
		return 0;
	} catch (...) { return RCS_FILE_CANNOT_ACCESS; }
}

///////////////////////////////////////////////////////////////////////////////
// Loglevel

void SetLogLevel(unsigned int loglevel) { state.loglevel = static_cast<LogLevel>(loglevel); }
unsigned int GetLogLevel() { return static_cast<unsigned int>(state.loglevel); }

///////////////////////////////////////////////////////////////////////////////
// RCS IO

int GetInput(unsigned int module, unsigned int port) {
	if (state.rcs == RcsState::stopped)
		return RCS_NOT_STARTED;
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	try {
		return modules[module]->rcsGetInput(port);
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

int GetOutput(unsigned int module, unsigned int port) {
	if (state.rcs == RcsState::stopped)
		return RCS_NOT_STARTED;
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	try {
		return modules[module]->rcsGetOutput(port);
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

int SetOutput(unsigned int module, unsigned int port, int _state) {
	if (state.rcs == RcsState::stopped)
		return RCS_NOT_STARTED;
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	try {
		return modules[module]->rcsSetOutput(port, _state);
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

int GetInputType(unsigned int module, unsigned int port) {
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	try {
		return modules[module]->rcsGetInputType(port);
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

int GetOutputType(unsigned int module, unsigned int port) {
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	try {
		return modules[module]->rcsGetOutputType(port);
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

///////////////////////////////////////////////////////////////////////////////
// Module questionaries

unsigned int GetModuleCount() {
	size_t count = 0;
	for (const auto& module : modules)
		if (module != nullptr)
			count++;
	return count;
}

bool IsModule(unsigned int module) {
	return (module < MAX_MODULES) && (modules[module] != nullptr);
}

unsigned int GetMaxModuleAddr() {
	return MAX_MODULES-1;
}

bool IsModuleFailure(unsigned int module) {
	return (module < MAX_MODULES) && (modules[module] != nullptr) && (modules[module]->state != "active");
}

int GetModuleTypeStr(unsigned int module, char16_t *type, unsigned int typeLen) {
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	StrUtil::strcpy<char16_t>(reinterpret_cast<const char16_t *>(modules[module]->type_str.utf16()), type, typeLen);
	return 0;
}

int GetModuleName(unsigned int module, char16_t *name, unsigned int nameLen) {
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	StrUtil::strcpy<char16_t>(reinterpret_cast<const char16_t *>(modules[module]->name.utf16()), name, nameLen);
	return 0;
}

int GetModuleFW(unsigned int module, char16_t *fw, unsigned int fwLen) {
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	StrUtil::strcpy<char16_t>(
		reinterpret_cast<const char16_t *>(
			(modules[module]->fw_version + " (B:" + modules[module]->bootloader_version + ")").utf16()
		),
		fw,
		fwLen);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Versions

bool ApiSupportsVersion(unsigned int version) {
	return std::find(API_SUPPORTED_VERSIONS.begin(), API_SUPPORTED_VERSIONS.end(), version) !=
	       API_SUPPORTED_VERSIONS.end();
}

int ApiSetVersion(unsigned int version) {
	if (!ApiSupportsVersion(version))
		return RCS_UNSUPPORTED_API_VERSION;

	rcs_api_version = version;
	return 0;
}

unsigned int GetDeviceVersion(char16_t *version, unsigned int versionLen) {
	const QString& sversion = "MTB-USB: " + mtbusb.fw_version;
	StrUtil::strcpy<char16_t>(reinterpret_cast<const char16_t *>(sversion.utf16()), version,
	                          versionLen);
	return 0;
}

unsigned int GetDriverVersion(char16_t *version, unsigned int versionLen) {
	const QString& sversion = VERSION;
	StrUtil::strcpy<char16_t>(reinterpret_cast<const char16_t *>(sversion.utf16()), version,
	                          versionLen);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// General library configuration

unsigned int GetModuleInputsCount(unsigned int module) {
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	try {
		return modules[module]->inputsCount();
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

unsigned int GetModuleOutputsCount(unsigned int module) {
	if ((module >= MAX_MODULES) || (modules[module] == nullptr))
		return RCS_MODULE_INVALID_ADDR;

	try {
		return modules[module]->outputsCount();
	} catch (...) { return RCS_GENERAL_EXCEPTION; }
}

///////////////////////////////////////////////////////////////////////////////
// Events binders

void BindBeforeOpen(StdNotifyEvent f, void *data) { events.bind(events.beforeOpen, f, data); }
void BindAfterOpen(StdNotifyEvent f, void *data) { events.bind(events.afterOpen, f, data); }
void BindBeforeClose(StdNotifyEvent f, void *data) { events.bind(events.beforeClose, f, data); }
void BindAfterClose(StdNotifyEvent f, void *data) { events.bind(events.afterClose, f, data); }
void BindBeforeStart(StdNotifyEvent f, void *data) { events.bind(events.beforeStart, f, data); }

void BindAfterStart(StdNotifyEvent f, void *data) { events.bind(events.afterStart, f, data); }
void BindBeforeStop(StdNotifyEvent f, void *data) { events.bind(events.beforeStop, f, data); }
void BindAfterStop(StdNotifyEvent f, void *data) { events.bind(events.afterStop, f, data); }
void BindOnError(StdErrorEvent f, void *data) { events.bind(events.onError, f, data); }
void BindOnLog(StdLogEvent f, void *data) { events.bind(events.onLog, f, data); }

void BindOnInputChanged(StdModuleChangeEvent f, void *data) { events.bind(events.onInputChanged, f, data); }
void BindOnOutputChanged(StdModuleChangeEvent f, void *data) { events.bind(events.onOutputChanged, f, data); }
void BindOnScanned(StdNotifyEvent f, void *data) { events.bind(events.onScanned, f, data); }

///////////////////////////////////////////////////////////////////////////////

} // namespace MtbNetLib
