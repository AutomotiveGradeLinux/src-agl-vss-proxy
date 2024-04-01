/*
 * Copyright (C) 2022,2023 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROXY_SERVICE_H
#define _PROXY_SERVICE_H

#include <mutex>
#include <glib.h>

#include "KuksaConfig.h"
#include "KuksaClient.h"
#include "MqttConfig.h"
#include "MqttClient.h"
#include "SignalUpdateHandlers.h"

class ProxyService
{
public:
	ProxyService(const KuksaConfig &config,
		     const MqttConfig &mqttConfig,
		     const SignalUpdateHandlers &signalUpdateHandlers,
		     GMainLoop *loop = NULL);

	~ProxyService();

	bool start();

	// Callback for KuksaClient subscribe API reconnect
	static gboolean resubscribe_cb(gpointer data) {
		struct resubscribe_data *d = (struct resubscribe_data*) data;
		if (d && d->self) {
			((ProxyService*) d->self)->Resubscribe(d->request);
		}
		return FALSE;
	}

private:
	struct resubscribe_data {
		ProxyService *self;
		const SubscribeRequest *request;
	};

	GMainLoop *m_loop;
	KuksaConfig m_kuksaConfig;
	KuksaClient *m_kuksaClient;
	MqttConfig m_mqttConfig;
	MqttClient *m_mqttClient;
	SignalUpdateHandlers m_signalUpdateHandlers;

	void HandleSignalChange(const std::string &path, const Datapoint &dp);

	void HandleSignalSetError(const std::string &path, const Error &error);

	void HandleSubscribeDone(const SubscribeRequest *request, const Status &status);

	void Resubscribe(const SubscribeRequest *request);
};

#endif // _PROXY_SERVICE_H
