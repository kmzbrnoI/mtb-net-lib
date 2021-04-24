#include "main.h"

namespace MtbNetLib {

AppThread main_thread;
MtbUsb mtbusb;
std::array<std::unique_ptr<MtbModule>, MAX_MODULES> modules;
DaemonClient daemonClient;
Settings settings;
State state;

}; // MtbNetLib
