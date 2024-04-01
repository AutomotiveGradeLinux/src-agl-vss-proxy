/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

#include "ParseConfig.h"

void readFile(const std::string &filename, std::string &data)
{
	data.clear();
	try {
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		data = buffer.str();
	} catch (const std::exception &e) {
		data.clear();
	}
}

bool ParseGlobalConfig(const YAML::Node &globalConfigYaml, GlobalConfig &globalConfig)
{
	std::string clientId = DEFAULT_CLIENT_ID;
	if (globalConfigYaml["client-id"]) {
		clientId = globalConfigYaml["client-id"].as<std::string>();
		if (clientId.empty()) {
			std::cerr << "Invalid client ID" << std::endl;
			return false;
		}
	}

	unsigned verbose = 0;
	std::string verboseStr;
	if (globalConfigYaml["verbose"])
		verboseStr = globalConfigYaml["verbose"].as<std::string>();
	if (!verboseStr.empty()) {
		if (verboseStr == "true" || verboseStr == "1")
			verbose = 1;
		if (verboseStr == "2")
			verbose = 2;
	}

	globalConfig = GlobalConfig(clientId, verbose);

	return true;
}

bool ParseKuksaConfig(const YAML::Node &kuksaConfigYaml, KuksaConfig &kuksaConfig)
{
	std::string hostname = "localhost";
	if (kuksaConfigYaml["hostname"]) {
		hostname = kuksaConfigYaml["hostname"].as<std::string>();
		if (hostname.empty()) {
			std::cerr << "Invalid server hostname" << std::endl;
			return false;
		}
	}

	unsigned port = DEFAULT_KUKSA_PORT;
	if (kuksaConfigYaml["port"]) {
		port = kuksaConfigYaml["port"].as<unsigned>();
		if (port == 0) {
			std::cerr << "Invalid server port" << std::endl;
			return false;
		}
	}

	bool useTls = false;
	if (kuksaConfigYaml["use-tls"]) {
		useTls = kuksaConfigYaml["use-tls"].as<bool>();
	}

	std::string caCert;
	std::string tlsServerName;
	if (useTls) {
		std::string caCertFile = DEFAULT_KUKSA_CA_CERT_FILE;
		if (kuksaConfigYaml["ca-certificate"]) {
			caCertFile = kuksaConfigYaml["ca-certificate"].as<std::string>();
			if (caCertFile.empty()) {
				std::cerr << "Invalid CA certificate filename" << std::endl;
				return false;
			}
		}

		if (!caCertFile.empty()) {
			readFile(caCertFile, caCert);
			if (caCert.empty()) {
				std::cerr << "Invalid CA certificate file" << std::endl;
				return false;
			}
		}

		if (kuksaConfigYaml["tls-server-name"]) {
			tlsServerName = kuksaConfigYaml["tls-server-name"].as<std::string>();
			if (tlsServerName.empty()) {
				std::cerr << "Invalid TLS server name" << std::endl;
				return false;
			}
		}
	}

	std::string authTokenFilename;
	if (kuksaConfigYaml["authorization"]) {
		authTokenFilename = kuksaConfigYaml["authorization"].as<std::string>();
		if (authTokenFilename.empty()) {
			std::cerr << "Invalid authorization token filename" << std::endl;
			return false;
		}
	}
	std::string authToken;
	if (!authTokenFilename.empty()) {
		readFile(authTokenFilename, authToken);
		if (authToken.empty()) {
			std::cerr << "Invalid authorization token file" << std::endl;
			return false;
		}
	}

	kuksaConfig = KuksaConfig(hostname,
				  port,
				  useTls,
				  caCert,
				  tlsServerName,
				  authToken);

	return true;
}

bool ParseMqttConfig(const YAML::Node &mqttConfigYaml, MqttConfig &mqttConfig)
{
	std::string hostname = "localhost";
	if (mqttConfigYaml["hostname"]) {
		hostname = mqttConfigYaml["hostname"].as<std::string>();
		if (hostname.empty()) {
			std::cerr << "Invalid MQTT server hostname" << std::endl;
			return false;
		}
	}

	unsigned port = DEFAULT_MQTT_PORT;
	bool defaultPort = true;
	if (mqttConfigYaml["port"]) {
		port = mqttConfigYaml["port"].as<unsigned>();
		if (port == 0) {
			std::cerr << "Invalid MQTT server port" << std::endl;
			return false;
		}
		defaultPort = false;
	}

	unsigned keepalive = DEFAULT_MQTT_KEEPALIVE;
	if (mqttConfigYaml["keepalive"]) {
		port = mqttConfigYaml["keepalive"].as<unsigned>();
	}

	std::string username;
	if (mqttConfigYaml["username"]) {
		username = mqttConfigYaml["username"].as<std::string>();
		if (username.empty()) {
			std::cerr << "Invalid MQTT username" << std::endl;
			return false;
		}
	}

	std::string password;
	if (mqttConfigYaml["password"]) {
		password = mqttConfigYaml["password"].as<std::string>();
		if (password.empty()) {
			std::cerr << "Invalid MQTT password" << std::endl;
			return false;
		}
	}

	std::string clientId = DEFAULT_MQTT_CLIENT_ID;
	if (mqttConfigYaml["client-id"]) {
		clientId = mqttConfigYaml["client-id"].as<std::string>();
		if (clientId.empty()) {
			std::cerr << "Invalid MQTT client ID" << std::endl;
			return false;
		}
	}

	bool cleanOnDisconnect = false;
	if (mqttConfigYaml["clean-on-disconnect"]) {
		cleanOnDisconnect = mqttConfigYaml["clean-on-disconnect"].as<bool>();
	}

	std::string topic = DEFAULT_MQTT_TOPIC;
	if (mqttConfigYaml["topic"]) {
		topic = mqttConfigYaml["topic"].as<std::string>();
		if (topic.empty()) {
			std::cerr << "Invalid MQTT topic" << std::endl;
			return false;
		}
	}

	unsigned qos = 0;
	if (mqttConfigYaml["qos"]) {
		qos = mqttConfigYaml["qos"].as<unsigned>();
		if (qos > 2) {
			std::cerr << "Invalid MQTT QoS" << std::endl;
			return false;
		}
	}

	bool retain = true;
	if (mqttConfigYaml["retain"]) {
		retain = mqttConfigYaml["retain"].as<bool>();
	}

	bool useTls = false;
	if (mqttConfigYaml["use-tls"]) {
		useTls = mqttConfigYaml["use-tls"].as<bool>();
	}

	std::string caCertFile;
	bool verifyServerHostname = true;
	std::string clientCertFile;
	std::string clientKeyFile;
	if (useTls) {
		if (defaultPort)
			port = DEFAULT_MQTT_TLS_PORT;

		if (mqttConfigYaml["ca-certificate"]) {
			caCertFile = mqttConfigYaml["ca-certificate"].as<std::string>();
			if (caCertFile.empty()) {
				std::cerr << "Invalid MQTT CA certificate filename" << std::endl;
				return false;
			}
		}
		// Mosquitto takes the cert filename as opposed to the
		// certificate itself, so no need to read it in here.

		if (mqttConfigYaml["verify-server-hostname"]) {
			verifyServerHostname = mqttConfigYaml["verify-server-hostname"].as<bool>();
		}

		if (mqttConfigYaml["client-certificate"]) {
			clientCertFile = mqttConfigYaml["client-certificate"].as<std::string>();
			if (clientCertFile.empty()) {
				std::cerr << "Invalid MQTT client certificate filename" << std::endl;
				return false;
			}
		}

		if (mqttConfigYaml["client-key"]) {
			clientKeyFile = mqttConfigYaml["client-key"].as<std::string>();
			if (clientKeyFile.empty()) {
				std::cerr << "Invalid MQTT client key filename" << std::endl;
				return false;
			}
		}
	}

	mqttConfig = MqttConfig(hostname,
				port,
				keepalive,
				username,
				password,
				clientId,
				cleanOnDisconnect,
				topic,
				qos,
				retain,
				useTls,
				caCertFile,
				verifyServerHostname,
				clientCertFile,
				clientKeyFile);

	return true;
}

bool ParseSignalsConfig(const YAML::Node &signalsConfigYaml, SignalUpdateHandlers &signalHandlers)
{
	if (!signalsConfigYaml.IsSequence())
		return false;

	bool rc = true;
	for (std::size_t i = 0; i < signalsConfigYaml.size(); i++) {
		YAML::Node signalYaml = signalsConfigYaml[i];
		if (!signalYaml.IsMap()) {
			rc = false;
			break;
		}
		if (signalYaml["signal"]) {
			std::string path = signalYaml["signal"].as<std::string>();
			if (!path.empty()) {
				std::shared_ptr<SignalUpdateHandler> handler =
					std::make_shared<PassthroughSignalUpdateHandler>(path);
				signalHandlers.addSignalUpdateHandler(path, handler);
			} else {
				rc = false;
				break;
			}
		}
	}

	return rc;
}

bool ParseConfig(const std::string &configFile,
		 GlobalConfig &globalConfig,
		 KuksaConfig &kuksaConfig,
		 MqttConfig &mqttConfig,
		 SignalUpdateHandlers &signalHandlers)
{
	bool rc = true;

	YAML::Node tmpYaml;
	try {
		tmpYaml = YAML::LoadFile(configFile);
	} catch (const std::exception &e) {
		std::cerr << "Invalid configuration " << configFile << std::endl;
		return false;
	}
	std::cout << "Reading configuration " << configFile << std::endl;
	const YAML::Node &configYaml = tmpYaml;

	rc = ParseGlobalConfig(configYaml, globalConfig);
	if (!rc)
		goto error;

	if (configYaml["kuksa"]) {
		rc = ParseKuksaConfig(configYaml["kuksa"], kuksaConfig);
		if (!rc)
			goto error;
	} else {
		kuksaConfig = KuksaConfig();
	}
	if (configYaml["mqtt"]) {
		rc = ParseMqttConfig(configYaml["mqtt"], mqttConfig);
		if (!rc)
			goto error;
	} else {
		mqttConfig = MqttConfig();
	}
	if (configYaml["signals"]) {
		rc = ParseSignalsConfig(configYaml["signals"], signalHandlers);
	} else {
		std::cerr << "Missing signals configuration " << std::endl;
		rc = false;
	}
error:
	return rc;
}

