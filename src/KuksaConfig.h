/*
 * Copyright (C) 2022-2024 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _KUKSA_CONFIG_H
#define _KUKSA_CONFIG_H

#include <string>

#define DEFAULT_KUKSA_PORT		55555
#define DEFAULT_KUKSA_CA_CERT_FILE	"/etc/kuksa-val/CA.pem"

class KuksaConfig
{
public:
	KuksaConfig();
		    
	explicit KuksaConfig(const std::string &hostname,
			     const unsigned port,
			     const bool useTls,
			     const std::string &caCert,
			     const std::string &tlsServerName,
			     const std::string &authToken);
	~KuksaConfig(){};

	std::string hostname() { return m_hostname; };
	unsigned port() { return m_port; };
	bool useTls() { return m_useTls; };
	std::string caCert() { return m_caCert; };
	std::string tlsServerName() { return m_tlsServerName; };
	std::string authToken() { return m_authToken; };
	bool valid() { return m_valid; };

private:
	std::string m_hostname;
	unsigned m_port;
	bool m_useTls;
	std::string m_caCert;
	std::string m_tlsServerName;
	std::string m_authToken;
	bool m_valid = false;
};

#endif // _KUKSA_CONFIG_H
