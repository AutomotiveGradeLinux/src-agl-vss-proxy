/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "SignalUpdateHandlers.h"

void SignalUpdateHandlers::addSignalUpdateHandler(const std::string &path, std::shared_ptr<SignalUpdateHandler> &handler)
{
	auto it = m_handlers.find(path);
	if (it == m_handlers.end()) {
		std::vector<std::shared_ptr<SignalUpdateHandler>> v;
		v.push_back(handler);
		m_handlers[path] = v;
	} else {
		m_handlers[path].push_back(handler);
	}
}

std::vector<std::string> SignalUpdateHandlers::getSignals()
{
	std::vector<std::string> signals;
	for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
		signals.push_back(it->first);
	}
	return signals;
}

void SignalUpdateHandlers::setClient(MqttClient *mqttClient)
{
	for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			std::shared_ptr<SignalUpdateHandler> handler = *it2;
			if (handler)
				handler->setClient(mqttClient);
		}
	}
}

void SignalUpdateHandlers::processSignalUpdate(const std::string &path, const Datapoint &dp)
{
	auto it = m_handlers.find(path);
	if (it == m_handlers.end())
		return;

	for (auto it2 = m_handlers[path].begin();
	     it2 != m_handlers[path].end();
	     ++it2) {
		std::shared_ptr<SignalUpdateHandler> handler = *it2;
		if (handler)
			handler->handleUpdate(dp);
	}
}
