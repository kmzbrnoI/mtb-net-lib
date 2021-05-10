#ifndef ERRORS_H
#define ERRORS_H

#include <cstdint>

constexpr size_t RCS_GENERAL_EXCEPTION = 1000;
constexpr size_t RCS_FILE_CANNOT_ACCESS = 1010;
constexpr size_t RCS_FILE_DEVICE_OPENED = 1011;

constexpr size_t RCS_MODULE_INVALID_ADDR = 1100;
constexpr size_t RCS_MODULE_UNKNOWN_TYPE = 1104;
constexpr size_t RCS_INVALID_SPEED = 1105;
constexpr size_t RCS_PORT_INVALID_NUMBER = 1103;
constexpr size_t RCS_PORT_INVALID_VALUE = 1106;
constexpr size_t RCS_INPUT_NOT_YET_SCANNED = 1108;
constexpr size_t RCS_MODULE_ALREADY_WRITING = 1110;

constexpr size_t RCS_ALREADY_OPENNED = 2001;
constexpr size_t RCS_CANNOT_OPEN_PORT = 2002;
constexpr size_t RCS_FIRMWARE_TOO_LOW = 2003;
constexpr size_t RCS_DEVICE_DISCONNECTED = 2004;
constexpr size_t RCS_SCANNING_NOT_FINISHED = 2010;
constexpr size_t RCS_NOT_OPENED = 2011;
constexpr size_t RCS_ALREADY_STARTED = 2012;
constexpr size_t RCS_OPENING_NOT_FINISHED = 2021;
constexpr size_t RCS_NOT_STARTED = 2031;

constexpr size_t RCS_MODULE_UPGRADING_FW = 3110;
constexpr size_t RCS_MODULE_IN_BOOTLOADER = 3111;
constexpr size_t RCS_MODULE_CONFIG_SETTING = 3112;
constexpr size_t RCS_MODULE_REBOOTING = 3113;
constexpr size_t RCS_MODULE_FWUPGD_ERROR = 3114;

constexpr size_t RCS_MODULE_FAILED = 3141;
constexpr size_t RCS_MODULE_RESTORED = 3142;

// Codes directly from RCS-USB errors
constexpr size_t RCS_MODULE_UNKNOWN_COMMAND = 0x1001;
constexpr size_t RCS_MODULE_UNSUPPORTED_COMMAND = 0x1002;
constexpr size_t RCS_MODULE_BAD_ADDRESS = 0x1003;
constexpr size_t RCS_SERIAL_PORT_CLOSED = 0x1010;
constexpr size_t RCS_USB_NO_RESPONSE = 0x1011;
constexpr size_t RCS_BUS_NO_RESPONSE = 0x1012;

constexpr size_t RCS_UNSUPPORTED_API_VERSION = 4000;

#endif
