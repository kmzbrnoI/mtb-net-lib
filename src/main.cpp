#include "main.h"

namespace MtbNetLib {

AppThread main_thread;
State state;
std::array<std::unique_ptr<MtbModule>, MAX_MODULES> modules;
DaemonClient daemonClient;

}; // MtbNetLib
