#ifndef MODULE_MTB_UNIS_H
#define MODULE_MTB_UNIS_H

#include "module.h"

namespace MtbNetLib {

constexpr size_t UNIS_IN_CNT = 16;
constexpr size_t UNIS_OUT_CNT = 28;

class MtbUnis : public MtbModule {
private:
	void restoreOutputs() const;

public:
	uint16_t inputs;
	std::array<QJsonObject, UNIS_OUT_CNT> outputsConfirmed;
	QJsonObject config;

	MtbUnis() = default;
	~MtbUnis() override = default;

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
