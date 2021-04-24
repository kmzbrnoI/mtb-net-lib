#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>
#include <QObject>
#include <memory>
#include "modules/module.h"
#include "client.h"
#include "settings.h"

namespace MtbNetLib {

enum class LogLevel {
	None = 0,
	Error = 1,
	Warning = 2,
	Info = 3,
	Commands = 4,
	RawData = 5,
	Debug = 6,
};

void log(const QString&, LogLevel);

struct MtbUsb {
	bool connected = false;
	size_t type;
	size_t speed;
	QString fw_version;
	QString proto_version;
};

enum class RcsState {
	closed = 0,
	opening = 1,
	stopped = 2,
	started = 4,
};

struct State {
	LogLevel loglevel = LogLevel::Info;
	RcsState rcs = RcsState::closed;
};

// Dirty magic for Qt's event loop
// This class should be created first
struct AppThread {
	AppThread() {
		if (qApp == nullptr) {
			int argc = 0;
			QCoreApplication* app = new QCoreApplication(argc, nullptr);
			QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
			app->exec();
		}
	}
};

class LibMain : public QObject {
	Q_OBJECT

private:
	void daemonReceivedMtbUsb(const QJsonObject&);

private slots:
	void daemonConnected();
	void daemonDisconnected();
	void daemonReceived(const QJsonObject&);

public:
	LibMain();

};

extern AppThread main_thread;
extern MtbUsb mtbusb;
extern std::array<std::unique_ptr<MtbModule>, MAX_MODULES> modules;
extern DaemonClient daemonClient;
extern Settings settings;
extern State state;
extern LibMain libMain;

} // namespace MtbNetLib

#endif
