/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SIGNAL_UPDATE_HANDLERS_H
#define _SIGNAL_UPDATE_HANDLERS_H

#include <string>
#include <map>
#include <vector>

#include "kuksa/val/v1/val.grpc.pb.h"
#include "SignalUpdateHandler.h"

// Just pull in the whole namespace since Datapoint contains a lot of
// definitions that may potentially be needed.
using namespace kuksa::val::v1;

class SignalUpdateHandlers
{
public:
	SignalUpdateHandlers() {};
	~SignalUpdateHandlers() {};

	void addSignalUpdateHandler(const std::string &path, std::shared_ptr<SignalUpdateHandler> &handler);
	std::vector<std::string> getSignals();
	void setClient(MqttClient *mqttClient);
	void processSignalUpdate(const std::string &path, const Datapoint &dp);

private:
	std::map<std::string, std::vector<std::shared_ptr<SignalUpdateHandler>>> m_handlers;
};

#endif // _SIGNAL_UPDATE_HANDLERS_H
