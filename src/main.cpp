/*
 * Copyright (C) 2022,2023 Konsulko Group
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <getopt.h>
#include <glib.h>
#include <glib-unix.h>

#include "ParseConfig.h"
#include "ProxyService.h"

#define DEFAULT_CONFIG		"/etc/agl-vss-proxy/config.yaml"

GlobalConfig g_config;

static gboolean quit_cb(gpointer user_data)
{
	GMainLoop *loop = (GMainLoop*) user_data;

	g_info("Quitting...");

	if (loop)
		g_idle_add(G_SOURCE_FUNC(g_main_loop_quit), loop);
	else
		exit(0);

	return G_SOURCE_REMOVE;
}

int main(int argc, char **argv)
{
	GMainLoop *loop = NULL;

	loop = g_main_loop_new(NULL, FALSE);
	if (!loop) {
		std::cerr << "Could not create GLib event loop" << std::endl;
		exit(2);
	}

	std::string configFile = DEFAULT_CONFIG;
	unsigned verbose = 0;
	struct option options[] = {
		{"config", required_argument, 0,  'c' },
		{"verbose", no_argument, 0,  'v' },
		{0, 0, 0, 0 }
	};
	int opt = 0;
	while ((opt = getopt_long(argc, argv,"c:v", options, NULL)) != -1) {
		switch (opt) {
		case 'c':
			configFile = optarg;
			break;
		case 'v':
			verbose++;
			break;
		default:
			std::cerr << "Invalid option" << std::endl;
			exit(1);
			break;
		}
	}

	KuksaConfig kuksaConfig;
	MqttConfig mqttConfig;
	SignalUpdateHandlers signalUpdateHandlers;
	if (!ParseConfig(configFile, g_config, kuksaConfig, mqttConfig, signalUpdateHandlers)) {
		std::cerr << "Could not parse configuration" << std::endl;
		exit(1);
	}
	if (verbose > 0)
		g_config.setVerbose(verbose);

	g_unix_signal_add(SIGTERM, quit_cb, (gpointer) loop);
	g_unix_signal_add(SIGINT, quit_cb, (gpointer) loop);

	ProxyService service(kuksaConfig, mqttConfig, signalUpdateHandlers, loop);
	if (service.start()) {
		g_main_loop_run(loop);
	}

	// Clean up
	g_main_loop_unref(loop);

	return 0;
}
