/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MQTT_CONFIG_H
#define _MQTT_CONFIG_H

#include <string>

#define DEFAULT_MQTT_PORT		1883
#define DEFAULT_MQTT_TLS_PORT		8883
#define DEFAULT_MQTT_KEEPALIVE		60
#define DEFAULT_MQTT_CLIENT_ID		"AGL-DEMO-Kdxm0Bzhaqe7c"
#define DEFAULT_MQTT_TOPIC		"AGL-V2C-TOPIC-ab9d6672-fe61-45b1-b027-867583fc8307"

class MqttConfig
{
public:
	MqttConfig();

	explicit MqttConfig(const std::string &hostname,
			    const unsigned port,
			    const unsigned keepalive,
			    const std::string &username,
			    const std::string &password,
			    const std::string &clientId,
			    const bool cleanOnDisconnect,
			    const std::string &topic,
			    const unsigned qos,
			    const bool retain,
			    const bool useTls,
			    const std::string &caCertFile,
			    const bool &verifyServerHostname,
    			    const std::string &clientCertFile,
			    const std::string &clientKeyFile);

	~MqttConfig(){};

	std::string hostname() { return m_hostname; };
	unsigned port() { return m_port; };
	unsigned keepalive() { return m_keepalive; };
	std::string username() { return m_username; };
	std::string password() { return m_password; };
	std::string clientId() { return m_clientId; };
	bool cleanOnDisconnect() { return m_cleanOnDisconnect; };
	std::string topic() { return m_topic; };
	unsigned qos() { return m_qos; };
	bool retain() { return m_retain; };
	bool useTls() { return m_useTls; };
	std::string caCertFile() { return m_caCertFile; };
	bool verifyServerHostname() { return m_verifyServerHostname; };
	std::string clientCertFile() { return m_clientCertFile; };
	std::string clientKeyFile() { return m_clientKeyFile; };
	bool valid() { return m_valid; };

private:
	std::string m_hostname;
	unsigned m_port;
	unsigned m_keepalive;
	std::string m_username;
	std::string m_password;
	std::string m_clientId;
	bool m_cleanOnDisconnect;
	std::string m_topic;
	unsigned m_qos;
	bool m_retain;
	bool m_useTls;
	std::string m_caCertFile;
	bool m_verifyServerHostname;
	std::string m_clientCertFile;
	std::string m_clientKeyFile;
	bool m_valid = false;
};

#endif // _MQTT_CONFIG_H
