/*
 * Copyright (C) 2022-2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "ProxyService.h"
#include "GlobalConfig.h"

ProxyService::ProxyService(const KuksaConfig &kuksaConfig,
			   const MqttConfig &mqttConfig,
			   const SignalUpdateHandlers &signalUpdateHandlers,
			   GMainLoop *loop) :
	m_loop(loop),
	m_kuksaConfig(kuksaConfig),
	m_mqttConfig(mqttConfig),
	m_signalUpdateHandlers(signalUpdateHandlers)
{
	// Create MQTT client
	m_mqttClient = new MqttClient(m_mqttConfig);
	if (m_mqttClient)
		m_signalUpdateHandlers.setClient(m_mqttClient);
}

ProxyService::~ProxyService()
{
	delete m_kuksaClient;
	delete m_mqttClient;
}

bool ProxyService::start() {
	if (!m_mqttClient)
		return false;

	if (!m_mqttClient->start())
		return false;

	// Create gRPC channel
	std::string host = m_kuksaConfig.hostname();
	host += ":";
	std::stringstream ss;
	ss << m_kuksaConfig.port();
	host += ss.str();

	std::cout << "Using KUKSA.val databroker " << host << std::endl;
	std::shared_ptr<grpc::Channel> channel;
	if (m_kuksaConfig.useTls() && !m_kuksaConfig.caCert().empty()) {
		std::cout << "Using TLS" << std::endl;
		grpc::SslCredentialsOptions options;
		options.pem_root_certs = m_kuksaConfig.caCert();
		if (!m_kuksaConfig.tlsServerName().empty()) {
			grpc::ChannelArguments args;
			auto target = m_kuksaConfig.tlsServerName();
			std::cout << "Overriding TLS target name with " << target << std::endl;
			args.SetString(GRPC_SSL_TARGET_NAME_OVERRIDE_ARG, target);
			channel = grpc::CreateCustomChannel(host, grpc::SslCredentials(options), args);
		} else {
			channel = grpc::CreateChannel(host, grpc::SslCredentials(options));
		}
	} else {
		channel = grpc::CreateChannel(host, grpc::InsecureChannelCredentials());
	}

	// Wait for the channel to be ready
	std::cout << "Waiting for databroker gRPC channel" << std::endl;
	while (!channel->WaitForConnected(std::chrono::system_clock::now() +
					  std::chrono::milliseconds(500))) ;
	std::cout << "Databroker gRPC channel ready" << std::endl;

	// Create Kuksa client
	bool rc = false;
	m_kuksaClient = new KuksaClient(channel, m_kuksaConfig);
	if (m_kuksaClient) {
		// Listen to signal updates
		std::map<std::string, bool> signals;
		auto handledSignals = m_signalUpdateHandlers.getSignals();
		for (auto it = handledSignals.begin(); it != handledSignals.end(); ++it) {
			std::cout << "Subscribing to " << *it << std::endl;
			signals[*it] = false;
		}
		m_kuksaClient->subscribe(signals,
					 [this](const std::string &path, const Datapoint &dp) {
						 HandleSignalChange(path, dp);
					 },
					 [this](const SubscribeRequest *request, const Status &s) {
						 HandleSubscribeDone(request, s);
					 });
		rc = true;
	}
	return rc;
}

// Private

void ProxyService::HandleSignalChange(const std::string &path, const Datapoint &dp)
{
	if (g_config.verbose() > 1)
		std::cout << "ProxyService::HandleSignalChange: Value received for " << path << std::endl;

	m_signalUpdateHandlers.processSignalUpdate(path, dp);
}

void ProxyService::HandleSignalSetError(const std::string &path, const Error &error)
{
	std::cerr << "Error setting " << path << ": " << error.code() << " - " << error.reason() << std::endl;
}

void ProxyService::HandleSubscribeDone(const SubscribeRequest *request, const Status &status)
{
	if (g_config.verbose())
		std::cout << "Subscribe status = " << status.error_code() <<
			" (" << status.error_message() << ")" << std::endl;

	if (status.error_code() == grpc::CANCELLED) {
		if (g_config.verbose())
			std::cerr << "Subscribe canceled, assuming shutdown" << std::endl;
		return;
	}

	// Queue up a resubcribe via the GLib event loop callback
	struct resubscribe_data *data = new (struct resubscribe_data);
	if (!data) {
		std::cerr << "Could not create resubcribe_data" << std::endl;
		exit(1);
	}
	data->self = this;
	// Need to copy request since the one we have been handed is from the
	// finished subscribe and will be going away.
	data->request = new SubscribeRequest(*request);
	if (!data->request) {
		std::cerr << "Could not create resubscribe SubscribeRequest" << std::endl;
		exit(1);
	}

	// NOTE: Waiting 100 milliseconds for now; it is possible that some
	//       randomization and/or back-off may need to be added if many
	//       subscribes are active, or switching to some other resubscribe
	//       scheme altogether (e.g. post subscribes to a thread that waits
	//       for the channel to become connected again).
	g_timeout_add_full(G_PRIORITY_DEFAULT,
			   100,
			   resubscribe_cb,
			   data,
			   NULL);
}

void ProxyService::Resubscribe(const SubscribeRequest *request)
{
	if (!(m_kuksaClient && request))
		return;

	m_kuksaClient->subscribe(request,
				 [this](const std::string &path, const Datapoint &dp) {
					 HandleSignalChange(path, dp);
				 },
				 [this](const SubscribeRequest *request, const Status &s) {
					 HandleSubscribeDone(request, s);
				 });
}
