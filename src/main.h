#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>

namespace MtbNetLib {

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


}; // namespace MtbNetLib

#endif
