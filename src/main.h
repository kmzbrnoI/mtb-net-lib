#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>
#include <memory>
#include "modules/module.h"
#include "client.h"

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

struct State {
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

extern AppThread main_thread;
extern State state;
extern std::array<std::unique_ptr<MtbModule>, MAX_MODULES> modules;
extern DaemonClient daemonClient;


}; // namespace MtbNetLib

#endif
