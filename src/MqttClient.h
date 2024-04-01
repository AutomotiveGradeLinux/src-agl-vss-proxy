/*
 * Copyright (C) 2019,2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <string>
#include <atomic>
#include <mosquitto.h>

#include "MqttConfig.h"

class MqttClient
{
public:
	MqttClient(const MqttConfig &config);
	~MqttClient();

	bool start();

	bool connected() {
		bool connected;
		connected = m_connected.load();
		return connected;
	}

	int publish(const std::string &topic, const char *data, const int len, const unsigned qos, const bool retain);
	int publish(const std::string &topic, const char *data, const int len);
	int publish(const char *data, const int len);

private:
	static void onConnect(struct mosquitto *mosq, void *obj, int rc);
	static void onDisconnect(struct mosquitto *mosq, void *obj, int rc);
	void handleConnect(int rc);
	void handleDisconnect(int rc);

	MqttConfig m_config;
	mosquitto *m_mosq = NULL;
	int m_loop = -1;
	std::atomic<bool> m_connected{false};
};

#endif // MQTTCLIENT_H
