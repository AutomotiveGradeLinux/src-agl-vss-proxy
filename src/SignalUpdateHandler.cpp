/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>

#include "SignalUpdateHandler.h"
#include "vss-notification.pb.h"
#include "GlobalConfig.h"

void PassthroughSignalUpdateHandler::handleUpdate(const Datapoint &dp)
{
	if (!m_mqttClient)
		return;

	if (!m_mqttClient->connected()) {
		if (g_config.verbose() > 1)
			std::cerr << "Not connected, dropping signal update" << std::endl;
		return;
	}
	
	agl::SignalUpdateNotification notification;
	notification.set_clientid(g_config.clientId());
	auto entry = notification.add_signals();
	entry->set_path(m_path);
	if (dp.has_timestamp()) {
		auto timestamp = new google::protobuf::Timestamp(dp.timestamp());
		entry->set_allocated_timestamp(timestamp);
	}

	bool handled = true;
	if (dp.has_int32()) {
		entry->set_int32(dp.int32());
	} else if (dp.has_uint32()) {
		entry->set_uint32(dp.uint32());
	} else if (dp.has_int64()) {
		entry->set_int64(dp.int64());
	} else if (dp.has_uint64()) {
		entry->set_uint64(dp.uint64());
	} else if (dp.has_bool_()) {
		entry->set_bool_(dp.bool_());
	} else if (dp.has_float_()) {
		entry->set_float_(dp.float_());
	} else if (dp.has_double_()) {
		entry->set_double_(dp.double_());
	} else if (dp.has_string()) {
		entry->set_string(dp.string());
	} else {
		// Array value types not handled at present
		handled = false;
	}

	if (handled) {
		if (g_config.verbose() > 1)
			std::cout << "Passing though signal " << m_path << " update" << std::endl;
		std::string bytes = notification.SerializeAsString();
		const char* array = bytes.data();
		int size = bytes.size();
		int rc = m_mqttClient->publish(array, size);
		if (g_config.verbose() > 1)
			std::cout << "MQTT publish returned " << rc << std::endl;
	}
}
