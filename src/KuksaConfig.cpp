/*
 * Copyright (C) 2022-2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <exception>
#include "KuksaConfig.h"

KuksaConfig::KuksaConfig() :
	m_hostname("localhost"),
	m_port(55555),
	m_useTls(false),
	m_caCert(""),
	m_tlsServerName(""),
	m_authToken("")

{
	m_valid = true;
}

KuksaConfig::KuksaConfig(const std::string &hostname,
			 const unsigned port,
			 const bool useTls,
			 const std::string &caCert,
			 const std::string &tlsServerName,
			 const std::string &authToken) :
	m_hostname(hostname),
	m_port(port),
	m_useTls(useTls),
	m_caCert(caCert),
	m_tlsServerName(tlsServerName),
	m_authToken(authToken)
{
	if (m_hostname.size() && m_port)
		m_valid = true;
	if (m_useTls && m_caCert.empty())
		m_valid = false;

}
