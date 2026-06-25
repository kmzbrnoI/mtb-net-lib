#ifndef MODULE_MTB_LED_H
#define MODULE_MTB_LED_H

/* MTB-LED module */

#include "module.h"

namespace MtbNetLib {

class MtbLed : public MtbModule {
private:
	void restoreOutputs() const;

public:
	static constexpr size_t IO_CNT = 32;

	uint32_t inputs;
	uint32_t outputsConfirmed;
	QJsonObject config;

	MtbLed() = default;
	~MtbLed() override = default;

	void daemonGotInfo(const QJsonObject&) override;
	void daemonInputsChanged(const QJsonObject&) override;
	void daemonOutputsChanged(const QJsonObject&) override;
	void daemonOutputsSet(const QJsonObject&) override;

	int rcsGetInput(unsigned int port) override;
	int rcsGetOutput(unsigned int port) override;
	int rcsSetOutput(unsigned int port, int state) override;
	int rcsGetInputType(unsigned int port) override;
	int rcsGetOutputType(unsigned int port) override;

	void resetConfig() override;
	void resetOutputsState() override;
	void resetInputsState() override;

	size_t inputsCount() const override;
	size_t outputsCount() const override;
};

} // namespace MtbNetLib

#endif
