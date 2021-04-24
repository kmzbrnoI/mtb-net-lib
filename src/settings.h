#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <map>

/* This file defines global settings of the application */

using Config = std::map<QString, std::map<QString, QVariant>>;

const Config DEFAULTS {
	{"server", {
		{"host", "127.0.0.1"},
		{"port", "3841"},
		{"keepAlive", true},
	}},
};

class Settings {
public:
	Settings();
	Config data;

	void load(const QString &filename, bool loadNonDefaults = true);
	void save(const QString &filename);

	std::map<QString, QVariant> &at(const QString &g);
	std::map<QString, QVariant> &operator[](const QString &g);

private:
	void loadDefaults();
};

#endif
