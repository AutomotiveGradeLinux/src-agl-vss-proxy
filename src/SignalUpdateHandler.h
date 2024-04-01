/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SIGNAL_UPDATE_HANDLER_H
#define _SIGNAL_UPDATE_HANDLER_H

#include <string>

#include "MqttClient.h"
#include "kuksa/val/v1/val.grpc.pb.h"

// Just pull in the whole namespace since Datapoint contains a lot of
// definitions that may potentially be needed.
using namespace kuksa::val::v1;

class SignalUpdateHandler
{
public:
	SignalUpdateHandler(const std::string &path) : m_path(path) {};
	virtual ~SignalUpdateHandler() {};

	void setClient(MqttClient *mqttClient) { m_mqttClient = mqttClient; };
	virtual void handleUpdate(const Datapoint &dp) = 0;

protected:
	std::string m_path;
	MqttClient *m_mqttClient = nullptr;
};

class PassthroughSignalUpdateHandler : public SignalUpdateHandler
{
public:
	PassthroughSignalUpdateHandler(const std::string &path) : SignalUpdateHandler(path) {};
	~PassthroughSignalUpdateHandler() {};

	void handleUpdate(const Datapoint &dp) override;
};

#endif // _SIGNAL_UPDATE_HANDLER_H
