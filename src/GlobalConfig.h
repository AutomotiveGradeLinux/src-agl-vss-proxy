/*
 * Copyright (C) 2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _GLOBAL_CONFIG_H
#define _GLOBAL_CONFIG_H

#include <string>

#define DEFAULT_CLIENT_ID		"AGL-V2C-932581de-7091-43bb-b8b2-e33437a5945f"

class GlobalConfig
{
public:
	GlobalConfig() {};
	explicit GlobalConfig(const std::string &clientId,
			      const unsigned verbose) :
		m_clientId(clientId),
		m_verbose(verbose) {};
	~GlobalConfig(){};

	std::string clientId() { return m_clientId; };
	unsigned verbose() { return m_verbose; };
	void setVerbose(const unsigned verbose) { m_verbose = verbose; };

private:
	std::string m_clientId;
	unsigned m_verbose = 0;
};

extern GlobalConfig g_config;

#endif // _GLOBAL_CONFIG_H
