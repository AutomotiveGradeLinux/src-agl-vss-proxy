/*
 * Copyright (C) 2019,2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include <cstring>

#include "MqttClient.h"
#include "GlobalConfig.h"

MqttClient::MqttClient(const MqttConfig &config) :
	m_config(config)
{
	mosquitto_lib_init();
	m_mosq = mosquitto_new(m_config.clientId().c_str(), m_config.cleanOnDisconnect(), this);

	mosquitto_connect_callback_set(m_mosq, onConnect);
	mosquitto_disconnect_callback_set(m_mosq, onDisconnect);
	
	if(m_config.username().size()) {
		mosquitto_username_pw_set(m_mosq,
					  m_config.username().c_str(),
					  m_config.password().c_str());
	}

	m_loop = mosquitto_loop_start(m_mosq);
	if(m_loop != MOSQ_ERR_SUCCESS){
		std::cerr << __FUNCTION__ << ": Unable to start loop, error = " << m_loop << std::endl;
	}
}

bool MqttClient::start()
{
	if (!(m_mosq && m_loop == MOSQ_ERR_SUCCESS))
		return false;

	if (!(m_config.username().empty() || m_config.password().empty())) {
		std::cout << "Using MQTT username & password" << std::endl;
		if (mosquitto_username_pw_set(m_mosq,
					      m_config.username().c_str(),
					      m_config.password().c_str()) != MOSQ_ERR_SUCCESS) {
			return false;
		}
	}

	if (m_config.useTls()) {
		std::cout << "Using MQTT TLS" << std::endl;
		if (!m_config.caCertFile().empty()) {
			std::cout << "Using MQTT TLS server certificate" << std::endl;
			char *clientCertFile = NULL;
			char *clientKeyFile = NULL;
			if (!(m_config.clientCertFile().empty() || m_config.clientKeyFile().empty())) {
				clientCertFile = strdup(m_config.clientCertFile().c_str());
				clientKeyFile = strdup(m_config.clientKeyFile().c_str());
				std::cout << "Using MQTT TLS client certificate" << std::endl;
			}
			if (mosquitto_tls_set(m_mosq,
					      m_config.caCertFile().c_str(),
					      NULL,
					      clientCertFile,
					      clientKeyFile,
					      NULL) != MOSQ_ERR_SUCCESS) {
				std::cerr << "Error configuring MQTT TLS support" << std::endl;
				free(clientCertFile);
				free(clientKeyFile);
				return false;
			}
		} else {
			// Use host's CA certificates
			mosquitto_int_option(m_mosq, MOSQ_OPT_TLS_USE_OS_CERTS, 1);
		}

		if (!m_config.verifyServerHostname()) {
			if (mosquitto_tls_insecure_set(m_mosq, true) != MOSQ_ERR_SUCCESS) {
				return false;
			}
		}
	}

	bool rc = true;
	std::cout << "Using MQTT server " << m_config.hostname() << ":" << m_config.port() << std::endl;
	if(mosquitto_connect_async(m_mosq,
				   m_config.hostname().c_str(),
				   m_config.port(),
				   m_config.keepalive())) {
		std::cerr << "Unable to start MQTT connection to " << m_config.hostname() << std::endl;
		rc = false;
	}

	return rc;
}

MqttClient::~MqttClient(void)
{
	mosquitto_disconnect(m_mosq);
	mosquitto_loop_stop(m_mosq, true);
	mosquitto_destroy(m_mosq);
	mosquitto_lib_cleanup();
}

int MqttClient::publish(const std::string &topic, const char *data, const int len, const unsigned qos, const bool retain)
{
	if (len <= 0 || qos > 2)
		return -1;

	return mosquitto_publish(m_mosq,
				 NULL,
				 topic.c_str(),
				 len,
				 data,
				 qos,
				 retain);
}

int MqttClient::publish(const std::string &topic, const char *data, const int len)
{
	if (len <= 0)
		return -1;

	return mosquitto_publish(m_mosq,
				 NULL,
				 topic.c_str(),
				 len,
				 data,
				 m_config.qos(),
				 m_config.retain());
}

int MqttClient::publish(const char *data, const int len)
{
	if (len <= 0)
		return -1;

	return mosquitto_publish(m_mosq,
				 NULL,
				 m_config.topic().c_str(),
				 len,
				 data,
				 m_config.qos(),
				 m_config.retain());
}

// Private

void MqttClient::onConnect(struct mosquitto *mosq, void *obj, int rc)
{
	if (!obj)
		return;

	MqttClient *p = (MqttClient*) obj;
	p->handleConnect(rc);
}

void MqttClient::onDisconnect(struct mosquitto *mosq, void *obj, int rc)
{
	if (!obj)
		return;

	MqttClient *p = (MqttClient*) obj;
	p->handleDisconnect(rc);
}

void MqttClient::handleConnect(int rc)
{
	m_connected = true;
	if (g_config.verbose())
		std::cerr << "MQTT Connected, rc = " << rc << std::endl;
}

void MqttClient::handleDisconnect(int rc)
{
	m_connected = false;
	if (g_config.verbose())
		std::cerr << "MQTT Disconnected, rc = " << rc << std::endl;
}
