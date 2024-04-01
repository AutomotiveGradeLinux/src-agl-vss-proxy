/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PARSE_CONFIG_H
#define _PARSE_CONFIG_H

#include <string>

#include "GlobalConfig.h"
#include "KuksaConfig.h"
#include "MqttConfig.h"
#include "SignalUpdateHandlers.h"

bool ParseConfig(const std::string &configFile,
		 GlobalConfig &globalConfig,
		 KuksaConfig &kuksaConfig,
		 MqttConfig &mqttConfig,
		 SignalUpdateHandlers &signalHandlers);

#endif // _PARSE_CONFIG_H
