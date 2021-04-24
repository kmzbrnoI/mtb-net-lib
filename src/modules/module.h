#ifndef MODULE_H
#define MODULE_H

#include <QTcpSocket>
#include <QJsonObject>

namespace MtbNetLib {

constexpr size_t MAX_MODULES = 256;

enum RcsPortInputType {
	iPlain = 0,
	iIr = 1,
};

enum RcsPortOutputType {
	oPlain = 0,
	oScom = 1,
};

enum MtbModuleType {
	Uknown = 0x00,
	Univ2ir = 0x10,
	Univ2noIr = 0x11,
	Univ40 = 0x15,
	Univ42 = 0x16,
};

class MtbModule {
public:
	uint8_t address;
	QString state;
	QString name;
	MtbModuleType type = MtbModuleType::Uknown;
	QString type_str;
	bool bootloader_int;
	bool bootlaoder_error;
	QString fw_version;
	QString proto_version;

	MtbModule(const QJsonObject& json) { this->daemonGotInfo(json); };

	virtual ~MtbModule() = default;

	virtual void daemonGotInfo(const QJsonObject& json) {
		this->address = json["address"].toInt();
		this->state = json["state"].toString();
		this->name = json["name"].toString();
		this->type = static_cast<MtbModuleType>(json["type_code"].toInt());
		this->type_str = json["type"].toString();
		this->bootloader_int = json["bootloader_intentional"].toBool();
		this->bootlaoder_error = json["bootloader_error"].toBool();
		this->fw_version = json["firmware_version"].toString();
		this->proto_version = json["protocol_version"].toString();
	}

	virtual void daemonInputsChanged(const QJsonObject&) {}
	virtual void daemonOutputsChanged(const QJsonObject&) {}
	virtual void daemonOutputsSet(const QJsonObject&) {}

	virtual int rcsGetInput(unsigned int port) { (void)port; return 0; }
	virtual int rcsGetOutput(unsigned int port) { (void)port; return 0; }
	virtual int rcsSetOutput(unsigned int port, int state) { (void)port; (void)state; return 0; }
	virtual int rcsGetInputType(unsigned int port) { (void)port; return 0; }
	virtual int rcsGetOutputType(unsigned int port) { (void)port; return 0; }

	virtual void resetConfig() {}
	virtual void resetState() {}

	virtual size_t inputsCount() const { return 0; }
	virtual size_t outputsCount() const { return 0; }
};

} // namespace MtbNetLib

#endif
