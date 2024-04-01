/*
 * Copyright (C) 2022-2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <exception>

#include "MqttConfig.h"

MqttConfig::MqttConfig() :
  	m_hostname("localhost"),
	m_port(DEFAULT_MQTT_PORT),
	m_keepalive(DEFAULT_MQTT_KEEPALIVE),
	m_username(""),
	m_password(""),
	m_clientId(DEFAULT_MQTT_CLIENT_ID),
	m_cleanOnDisconnect(false),
	m_topic(DEFAULT_MQTT_TOPIC),
	m_qos(0),
	m_retain(true),
	m_useTls(false),
	m_caCertFile(""),
	m_verifyServerHostname(""),
	m_clientCertFile(""),
	m_clientKeyFile("")
{
	m_valid = true;
}

MqttConfig::MqttConfig(const std::string &hostname,
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
		       const std::string &clientKeyFile) :
	m_hostname(hostname),
	m_port(port),
	m_keepalive(keepalive),
	m_username(username),
	m_password(password),
	m_clientId(clientId),
	m_cleanOnDisconnect(cleanOnDisconnect),
	m_topic(topic),
	m_qos(qos),
	m_retain(retain),
	m_useTls(useTls),
	m_caCertFile(caCertFile),
	m_verifyServerHostname(verifyServerHostname),
	m_clientCertFile(clientCertFile),
	m_clientKeyFile(clientKeyFile)
{
	if (m_hostname.size() && m_port)
		m_valid = true;
	if ((m_useTls && m_caCertFile.empty()) ||
	    m_topic.empty() ||
	    m_qos > 2)
		m_valid = false;
}
