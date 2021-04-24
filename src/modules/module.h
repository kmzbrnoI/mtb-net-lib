#ifndef MODULE_H
#define MODULE_H

#include <QTcpSocket>
#include <QJsonObject>

namespace MtbNetLib {

constexpr size_t MAX_MODULES = 256;

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

	MtbModule(uint8_t addr) : address(addr) {};
	virtual ~MtbModule() = default;

	virtual void daemonGotInfo(const QJsonObject&) {}
	virtual void daemonInputsChanged(const QJsonObject&) {}
	virtual void daemonOutputsChanged(const QJsonObject&) {}
	virtual void daemonOutputsSet(const QJsonObject&) {}

	virtual int rcsGetInput(unsigned int port) { (void)port; return 0; }
	virtual int rcsGetOutput(unsigned int port) { (void)port; return 0; }
	virtual int rcsSetOutput(unsigned int port, int state) { (void)port; (void)state; return 0; }
	virtual int rcsGetInputType(unsigned int port) { (void)port; return 0; }
	virtual int rcsGetOutputsType(unsigned int port) { (void)port; return 0; }

	virtual void resetConfig();
	virtual void resetState();
};

}; // namespace MtbNetLib

#endif
