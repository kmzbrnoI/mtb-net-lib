#ifndef MODULE_MTB_UNI_H
#define MODULE_MTB_UNI_H

/* MTB-UNI and MTB-UNIS module */

#include <vector>
#include "module.h"

namespace MtbNetLib {

constexpr size_t UNI_IN_CNT = 16;
constexpr size_t UNI_OUT_CNT = 16;
constexpr size_t UNIS_IN_CNT = 28;
constexpr size_t UNIS_OUT_CNT = 28;

class MtbUni : public MtbModule {
private:
	void restoreOutputs() const;

public:
	uint32_t inputs;
	std::vector<QJsonObject> outputsConfirmed;
	QJsonObject config;
	bool ir;

	MtbUni() = default;
	~MtbUni() override = default;

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
